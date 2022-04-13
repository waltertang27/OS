//  to do in 6.3.1
#include "systemcalls.h"

int32_t idt_flag; 


int32_t parent_id = 0; 
int32_t curr_id = 0;


extern void flush_tlb(); 

/*
DESCRIPTION: The halt system call terminates a process, returning the specified value to its parent process.
INPUTS: uint8_t status - current program status
OUTPUTS:
RETURN VALUE: returns -1 if halt fails or status if it does not 
SIDE EFFECTS: Terminate the current program and return to the previous program
*/
int32_t halt(uint8_t status)
{
    int32_t newStatus; 
    pcb_t * pcb, *parent ;
    uint32_t addr;
    // =============================== Restore parent data   ===============================

    pcb = get_cur_pcb();

    //If you are the last PID execute a new shell 
    if(pcb->process_id == 0){
        process_array[pcb->process_id] = 0; 
        execute((const uint8_t * )"shell");
    }

    // New PID is now the parent since you are halting the current process 
    curr_id = pcb->parent_id;
    parent = get_pcb(curr_id);
    process_array[pcb->process_id] = 0; 


    // =============================== Restore parent paging data   ===============================

    //Get physical memory and then Change Page table 
    addr = EIGHTMB + ((curr_id ) * PAGE_SIZE); // not sure if we need to minus 2 or not
    page_directory[USER_INDEX].page_table_addr = addr / ALIGN_BYTES;

    //Always flush the tlb before returning to a new program 
    flush_tlb(); 

    // =============================== Close any relevant FD's   ===============================
    int i;
    for(i = 0; i < FD_END; i++) {
        pcb->fd_array[i].flags = FREE;
    }


    tss.esp0 = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * curr_id);

    tss.ss0 = KERNEL_DS;
    

    // =============================== jump to execute return   ===============================
    
    // Call assembly program to jump back to execute for the original PCB
    int32_t ebpSave = pcb->save_ebp; 
    int32_t espSave = pcb->save_esp;



    newStatus = (uint32_t)status; 
    if(idt_flag){
        newStatus = 256; 
        idt_flag = 0; 
    }
    
    //Push paramaters and jump to execute 
     asm volatile(
        "movl %%edx, %%esp \n "
        "movl %%ecx, %%ebp \n "
        "jmp leaveExec \n "
        :
        : "a"(status), "d"(espSave), "c"(ebpSave)
        : "memory", "ebx"
        );

    //If code gets here something went horribly wrong 
    return -1;

}
/*
DESCRIPTION: loads and executes a new program, handing off processor to new program until it terminates
INPUTS: const uint8_t *command - sequence of words consisting of commands
                                - first word is file name to be exectued
OUTPUTS: The current program will be executed in the terminal
RETURN VALUE:  -1 if command cannot be executed
               256 if program dies by execption
               0 to 255 if program executes a halt system call 
SIDE EFFECTS: hands processor to new program until it terminates
*/
int32_t execute (const uint8_t* command){
    // ===============================    parsing    ===============================
    int command_size = strlen( (const int8_t * ) command);
    int i = 0;
    int j = 0;
    int spaces, error;
    uint32_t addr;
    uint8_t cmd[MAX_CMD_LINE_SIZE]; // again, size not sure
    uint8_t args[MAX_CMD_LINE_SIZE];
    uint8_t buf[BUF_SIZE];
    uint32_t returnVal; 
    memset((void *)cmd,0,MAX_CMD_LINE_SIZE );
    memset((void *)args,0,MAX_CMD_LINE_SIZE );

    INode_t * inode;
    dentry_t dentry;
    pcb_t * pcb;

    uint32_t eip_usr, esp_usr;

    /* 
        Part 1: Parsing command and args.
        cmd: read spaces until we have read a non space character. Then write the non spaces 
        characters to cmd until we read a space character.
        If all spaces, return fail.
    */
    while (command[i] == ' ' ){
        if (i == command_size - 1){
            return -1;
        }
        i++;
    }
    spaces = i;
    while (command[i] != ' ' && command[i] != '\0'){
        cmd[i - spaces] = command[i]; 
        i++;
    }


    /* args */

    memset((void *)args, 0, 32);
    //Remove trailing spaces
    while (command[i] == ' '){
         if (i == command_size - 1){
            return -1 ;
        }
        i++;
    }

        while (command[i] != '\0' && command[i] != ' ')
        {
            if(i < command_size){
                args[j] = command[i];
                i++;
                j++;
            }
            else{
                break; 
            }
        }
 
    // printf("Args: %s \n",args); 

    // =============================== check for executable ===============================

    /* 
    The first 4 bytes of the file represent a “magic number” that identifies the file as an executable. 
    We are checking to make sure these things we are executing are "executables"  
    */

    if (read_dentry_by_name(cmd, &dentry)  == -1) { // no file
        return -1;
    } else if (read_data(dentry.INodeNum, 0, buf, BUF_SIZE) == -1){ // data reading failure
        return -1;
    } else if (buf[0] != MAGIC_1){ // check for magic nums
        return -1;
    } else if (buf[1] != MAGIC_2){
        return -1;
    } else if (buf[2] != MAGIC_3){
        return -1;
    } else if (buf[3] != MAGIC_4){
        return -1;
    }

    // ===============================     Set up paging     ===============================

    while (curr_id < PROCESS_ARRAY_SIZE){
        if (process_array[curr_id] != 1){
            process_array[curr_id] = 1;
            break;
        }
        curr_id++;
    }
    // full array
    if (curr_id >= PROCESS_ARRAY_SIZE){
        printf("Maximum number of shells reached\n");
        return 1;
    }

    //Get the physical adress and set up paging acording to it 
    addr = EIGHTMB + ((curr_id ) * PAGE_SIZE); 
    page_directory[USER_INDEX].page_table_addr = addr / ALIGN_BYTES;

    // We are about to execute a new program with its own VRAM so tlb needs to be flushed again
    flush_tlb();

    //===============================  Load file into memory ===============================
    inode = (INode_t * )(startINode + dentry.INodeNum);
    uint8_t * top_img = (uint8_t * )PROCESS_ADDR ; 
    error = read_data(dentry.INodeNum, 0, top_img , inode->bLength);

    if (error == -1){
        return -1;
    }

    // ===============================      Create PCB       ===============================
    pcb = get_pcb(curr_id);
    pcb->process_id = curr_id;

    // Fill all 8 FD's with values
    for (i = 0; i < FD_ARRAY_SIZE; i++)
    {
        pcb->fd_array[i].jump_table = &null_op;    
        pcb->fd_array[i].file_position = 0; 
        pcb->fd_array[i].flags = FREE;
        pcb->fd_array[i].inode = 0; 
    }

    //SDIN and flags need to be switched to in use and we need to assign their jump tables for open and close 
    pcb->fd_array[STDIN].flags = IN_USE;
    pcb->fd_array[STDOUT].flags = IN_USE;

    pcb->fd_array[STDOUT].jump_table = &stdout_fileop;
    pcb->fd_array[STDIN].jump_table = &stdin_fileop;

    strncpy((int8_t * ) pcb->pcb_cmd, (int8_t * ) cmd, MAX_FILE_NAME);
    strncpy((int8_t *)pcb->pcb_arg, (const int8_t *)args, MAX_FILE_NAME);

        // =============================== Prepare for Context Switch ===============================

    uint8_t eip_value[SIZE_OF_INT32];
    read_data(dentry.INodeNum, STARTEXEC, eip_value, SIZE_OF_INT32);

    // Set stack pointer to the bottom of the 4 MB page
    esp_usr = PAGE_SIZE - SIZE_OF_INT32 + USER_V;
    eip_usr = * ((int * )eip_value);

    // TSS initalization
    pcb->active = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * curr_id);
    tss.esp0 = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * curr_id);
    tss.ss0 = KERNEL_DS;
    


    // Save old EBP and ESP
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    pcb->save_ebp = saved_ebp;
    pcb->save_esp = saved_esp;
    pcb->usr_eip = eip_usr;
    pcb->usr_esp = esp_usr;

    // =============================== Push IRET context to kernel stack  ===============================
    // Set the registers that we want to pop to the correct values
    // Possible reasons it doesnt work: segment registers need to be set
    sti();

    // Push an iret stack to switch to our user program in the following order
    // USER_DS, ESP_USR, EFLAGS, USER_CS, EIP
    //Also set the value of DS(Might happen when iret is called but we did it to be safe )
    asm volatile (
        "andl $0x00FF, %%edx \n "
        "movw %%dx,%%ds \n "
        "pushl %%edx \n"
        "pushl %%ecx \n" 
        "pushfl      \n"
        "popl %%ecx  \n"
        "orl $0x0200, %%ecx   \n"
        "pushl %%ecx \n"
        "pushl %%ebx \n"
        "pushl %%eax \n"
        "iret \n"
        :
        : "a"(eip_usr), "b"(USER_CS), "c"(esp_usr), "d"(USER_DS)
        : "memory" 
    );
    //Label for halt to jump to after 
    asm volatile("leaveExec: \n");

    // Move the return value from halt(status) into a value we can return in C
    asm volatile("movl %%eax, %%ebx \n"
    :   "=b"(returnVal)
    : 
    : "memory"
    );

    return returnVal; // value between 0 and 255
}


/*
DESCRIPTION: read system call; reads from keyboard, rtc, file, directory, etc.
INPUTS: int32_t fd - file descriptor to read
        void *buf - buffer to read data to
        int32_t nbytes - number of bytes to read
OUTPUTS: none
RETURN VALUE: number of bytes read
SIDE EFFECTS: 
*/
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    // fail
    //dentry_t dentry;
    //cli();
    if (fd < 0 || fd > FD_END || nbytes < 0 || buf == NULL) { // check 1. if the file descriptor is in scope
                                                              //       2. if the buffer exists
                                                              //       3. if the bytes to read exist
        return -1;
    }
    pcb_t * pcb = get_cur_pcb(); // get the current pcb block
    if(pcb->fd_array[fd].flags == FREE) { // if it is not in use, then return invalid
        return -1;
    }
    else {
        int32_t val = pcb->fd_array[fd].jump_table->read(fd, buf, nbytes);
        return val;
    }
    
}
/*
DESCRIPTION: write system call; writes from keyboard, rtc, file, directory, etc.
INPUTS: int32_t fd - file descriptor to write
        void *buf - buffer to write data to
        int32_t nbytes - number of bytes to write
OUTPUTS: none
RETURN VALUE: number of bytes written
SIDE EFFECTS: 
*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    if (fd < 0 || fd > FD_END || buf == NULL || nbytes < 0)  // check 1. if the file descriptor is in scope
                                                             //       2. if the buffer exists
                                                             //       3. if the bytes to write exist
        return -1;
    
    pcb_t * pcb = get_cur_pcb();  // get the current pcb block
    if (pcb->fd_array[fd].flags == FREE){  // if it is not in use, then return invalid
        return -1;
    }

    int32_t i = pcb->fd_array[fd].jump_table->write(fd, buf, nbytes);

    return i;
}
/*
DESCRIPTION: find directory entry corresponding to file and set up data to handle the file
INPUTS: const uint8_t *filename - pointer to filename of file
OUTPUTS: none
RETURN VALUE: position found
                -1 if fail
SIDE EFFECTS: 
*/
int32_t open (const uint8_t* filename){
    // get the current pcb block 
    pcb_t * pcb = get_cur_pcb(); 
    int32_t i;
    dentry_t dentry;
    if (filename == NULL){ // if the file does not exist, return -1
        return -1;
    }
    int error;
    // read into dentry
    error = read_dentry_by_name(filename, &dentry);
    if (error == -1)
        return -1;

    for(i = FD_START_INDEX; i < FD_END; i++) {
        if (pcb->fd_array[i].flags == FREE) {
            pcb->fd_array[i].file_position = 0; 
            pcb->fd_array[i].flags = IN_USE; // if it is not in use, turn it to in use
            pcb->fd_array[i].inode = dentry.INodeNum;
            
            if (dentry.fileType == 0) { // rtc
                pcb->fd_array[i].jump_table = &rtc_op; // rtc operation table
            }
    
            else if (dentry.fileType == 1) {// directory
                pcb->fd_array[i].jump_table = &dir_op; // directory operation table
            }
        
            else if (dentry.fileType == 2) {// file
                pcb->fd_array[i].jump_table = &file_op; // file operation table
            }
            return i;
        }
    }
    // fail
    return -1;
}
/*
DESCRIPTION: closes specified file descriptor
INPUTS: int32_t fd - file descriptor to close
OUTPUTS: none
RETURN VALUE: 0 if successful
              -1 if closing invalid descriptor
SIDE EFFECTS: 
*/
int32_t close (int32_t fd){
    if (fd < FD_START_INDEX || fd > FD_END)
        return -1;
    pcb_t * pcb = get_cur_pcb();
    if (pcb->fd_array[fd].flags == FREE) { // file not in use, invalid descriptor
        return -1;
        // if trying to close an unused file failed, return -1
        //if (pcb->fd_array[fd].close(fd) != pcb->fd_array[fd].flags) 
        //    return -1;
    }
    // closing the file
    pcb->fd_array[fd].inode = 0;
    pcb->fd_array[fd].flags = FREE;
    pcb->fd_array[fd].file_position = 0;

    return pcb->fd_array[fd].jump_table->close(fd);
}

/*
DESCRIPTION: inits the file_op tables for FD's that we will be using
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: sets the corresponding open/close/read/write functions
*/
void fileop_init(){
    stdin_fileop.close = terminal_close; 
    stdin_fileop.open = terminal_open; 
    stdin_fileop.read = terminal_read; 
    stdin_fileop.write = write_fail; 

    stdout_fileop.close = terminal_close;
    stdout_fileop.open = terminal_open; 
    stdout_fileop.read = read_fail; 
    stdout_fileop.write = terminal_write;

    null_op.close = close_fail;
    null_op.open = open_fail; 
    null_op.read = read_fail; 
    null_op.write = write_fail; 

    rtc_op.read = read_rtc;
    rtc_op.close = close_rtc;
    rtc_op.open = open_rtc;
    rtc_op.write = write_rtc;

    file_op.read = file_read;
    file_op.close = file_close;
    file_op.open = file_open;
    file_op.write = file_write;

    dir_op.read = directory_read;
    dir_op.close = directory_close;
    dir_op.open = directory_open;
    dir_op.write = directory_write;

}

/*
DESCRIPTION: Fills a buffer with the arguments for teh current pcb
INPUTS: buf - buffer to copy into
        nbytes: number of bytes to copy from arg

OUTPUTS: none
RETURN VALUE: 0 if it worked -1 if it didnt 
SIDE EFFECTS:
*/
int32_t getargs(uint8_t *buf, int32_t nbytes)
{
    if(buf == NULL)
        return -1; 
    
    pcb_t * curr = get_cur_pcb(); 
    
    if(*(curr->pcb_arg) == NULL)
        return -1; 

    strncpy((void *)buf,(const void *)curr->pcb_arg,nbytes); 
    
    return 0; 
}

int32_t vidmap (uint8_t** screen_start){
    // check if address is valid
    if (screen_start == NULL){
        return -1;
    }

    /* To avoid adding kernel-side exception handling for this sort of check, you can simply check whether the address falls
        within the address range covered by the single user-level page.  --Appendix B*/
    uint32_t screen_start_addr = (uint32_t)screen_start;
    if (screen_start_addr < USER_V){
        return -1;
    } else if (screen_start_addr > USER_V + PAGE_SIZE) {
        return -1;
    }

    page_directory[VIDMAP_INDEX].user_supervisor = 1;
    page_directory[VIDMAP_INDEX].present = 1;
    page_directory[VIDMAP_INDEX].page_size = 0;
    page_directory[VIDMAP_INDEX].page_size = (int32_t)video_mapping_pt / ALIGN_BYTES; // points to the video mapping page table
    video_mapping_pt[0].user_supervisor = 1;
    video_mapping_pt[0].present = 1;
    // video_mapping_pt[0].page_table_addr = ??? B8000

    flush_tlb();
    return 0;
}

int32_t set_handler (int32_t signum, void* handler_address){
    // apparently these are extra credit
    return -1;
}


int32_t sigreturn(void){
    return -1;
}


/*
DESCRIPTION: gets address of pcb corresponding to id
INPUTS: int32_t id - ID of pcb we want to find
 
OUTPUTS: none
RETURN VALUE: pointer to pcb corresponding to id
SIDE EFFECTS: 
*/
pcb_t * get_pcb(int32_t id){
	uint32_t addr = EIGHTMB - EIGHTKB * (id + 1);
	return (pcb_t * )addr;
}

/*
DESCRIPTION: gets the address to the current pcb
INPUTS: none
OUTPUTS: none
RETURN VALUE: pointer to current pcb
SIDE EFFECTS: 
*/
pcb_t * get_cur_pcb() {
	uint32_t addr = EIGHTMB - EIGHTKB * (curr_id + 1);
	return (pcb_t *) addr;
}


//In the case where an FD cannot read,write,open, or close we need to put something into the jump table so that is what these functins aim to do
int32_t read_fail(int32_t fd, void *buf, int32_t nbytes) { return -1 ;}
int32_t write_fail(int32_t fd, const void *buf, int32_t nbytes){return -1; }
int32_t open_fail(const uint8_t *filename) {return -1;}
int32_t close_fail(int32_t fd){ return -1; }

//FUnctions we will be writing in the future
int32_t vidmap (uint8_t** screen_start) {return -1;}
int32_t set_handler (int32_t signum, void* handler_address) {return -1;}
int32_t sigreturn (void) {return -1;}

