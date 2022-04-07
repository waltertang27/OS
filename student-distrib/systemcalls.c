//  to do in 6.3.1
#include "systemcalls.h"


#define MAX_CMD_LINE_SIZE 32 // not sure
#define BUF_SIZE 4

int32_t id = 0;
int32_t curr_id = 0;

int32_t halt(uint8_t status){
    /* The halt system call terminates a process, returning the specified value to its parent process. The system call handler
    itself is responsible for expanding the 8-bit argument from BL into the 32-bit return value to the parent program’s
    execute system call. Be careful not to return all 32 bits from EBX. This call should never return to the caller. */

    /*
        Restore parent data
        Restore parent paging
        Close any relevant FDs
        Jump to execute return
    */
    pcb_t * pcb;

    /* restore parent data */

    pcb = get_pcb(curr_id);
    curr_id = pcb->parent_id;

    /* restore parent paging */
    /* everytime we change paging we need to flush TLB */

    // flush tlb here
    // asm volatile (" 
    //     movl %cr3, %eax ;
    //     movl %eax, %cr3 ;
    //     ret
    // "
    // );

    

    // fail
    return -1;
}

int32_t execute (const uint8_t* command){
    /* The execute system call attempts to load and execute a new program, handing off the processor to the new program
        until it terminates. The command is a space-separated sequence of words. The first word is the file name of the
        program to be executed, and the rest of the command—stripped of leading spaces—should be provided to the new
        program on request via the getargs system call. The execute call returns -1 if the command cannot be executed,
        for example, if the program does not exist or the filename specified is not an executable, 256 if the program dies by an
        exception, or a value in the range 0 to 255 if the program executes a halt system call, in which case the value returned
        is that given by the program’s call to halt. -- appendix B */

    /*
        Parse args
        Check for executable
        Set up paging
        Load file into memory
        Create PCB
        Prepare for Context Switch
        Push IRET context to kernel stack
    */

    // ===============================    parsing    ===============================
    int command_size = strlen( (const int8_t * ) command);
    int i = 0;
    int spaces, error;
    uint32_t addr;
    uint8_t cmd[MAX_CMD_LINE_SIZE]; // again, size not sure
    uint8_t args[MAX_CMD_LINE_SIZE];
    uint8_t buf[BUF_SIZE];
    // uint8_t * memory;
    // uint8_t * inode;
    inode_t * inode;
    dentry_t dentry;
    pcb_t * pcb;

    /* 
        Part 1: Parsing command and args.
        cmd: read spaces until we have read a non space character. Then write the non spaces 
        characters to cmd until we read a space character.
        If all spaces, return fail.
    */
    while (command[i] == ' '){
        if (i == command_size - 1){
            return -1;
        }
        i++;
    }
    spaces = i;
    while (command[i] != ' '){
        cmd[i - spaces] = command[i];
        i++;
    }


    /* args */

    // check logic
    while (i < command_size){
        while (command[i] == ' '){
            if (i == command_size - 1){
                break;
            }
            i++;
            spaces++;
        }
        while (command[i] != ' '){
            args[i - spaces] = command[i];
            i++;
        }
        if (i != command_size - 1){
            args[i - spaces] = ' ';
        }
    }

    // =============================== check for executable ===============================

    /* about magic nums: The first 4 bytes of the file represent a “magic number” that identifies the file as an executable. 
        These bytes are, respectively, 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46. If the magic number is not present, the execute
        system call should fail. -- appendix C*/

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
    curr_id = FD_START_INDEX;
    while (curr_id < PROCESS_ARRAY_SIZE){
        if (process_array[curr_id] == 0){
            process_array[curr_id] = 1;
            break;
        }
        curr_id++;
    }
    // full array
    if (curr_id >= PROCESS_ARRAY_SIZE){
        return -1;
    }

    addr = PAGE_SIZE * curr_id;
    page_directory[USER_INDEX].page_table_addr = addr / ALIGN_BYTES;

    // NEED TO FLUSH TLB HERE

    //===============================  Load file into memory ===============================
    inode = (inode_t * )(startINode + dentry.INodeNum);
    error = read_data(dentry.INodeNum, 0, (uint8_t * )PROCESS_ADDR, inode->bLength);

    // ===============================      Create PCB       ===============================
    pcb = get_pcb(id);
    pcb->process_id = id;
    curr_id = id;
    id++;

    // remaining six file descriptors available
    for (i = 0; i < FD_ARRAY_SIZE; i++)
    {
        pcb->fd_array[i].jump_table = &null_op;    
        pcb->fd_array[i].file_position = 0; 
        pcb->fd_array[i].flags = FREE;
        pcb->fd_array[i].inode = 0; 
    }

    pcb->fd_array[STDIN].flags = IN_USE;
    pcb->fd_array[STDOUT].flags = IN_USE;

    pcb->fd_array[STDOUT].jump_table = &stdout_fileop;
    pcb->fd_array[STDIN].jump_table = &stdin_fileop;

    strncpy(&pcb->pcb_cmd, &cmd,32);

    // =============================== Prepare for Context Switch ===============================

    int8_t eip_value[3]; 
    read_data(dentry.INodeNum,24,eip_value,3);

    // Set stack pointer to the bottom of the 4 MB page
    // Ds register set to USER_DS
    // Apendix E stuff smtn related to tss
    // Save old EBP and ESP

    // =============================== Push IRET context to kernel stack  ===============================
    // Set the registers that we want to pop to the correct values
    // asm volatile ("
    //     pushw %ds ;
    //     pushl %esp ;
    //     pushl EFLAG ;
    //     pushl %es; 
    //     pushl %eip ; 
    //     iret 
    // ");


    return 172; // value between 0 and 255
}


int32_t read (int32_t fd, void* buf, int32_t nbytes){
    // fail
    return -1;
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    /*
        The write system call writes data to the terminal or to a device (RTC). In the case of the terminal, all data should
        be displayed to the screen immediately. In the case of the RTC, the system call should always accept only a 4-byte
        integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly. Writes to regular
        files should always return -1 to indicate failure since the file system is read-only. The call returns the number of bytes
        written, or -1 on failure.

    */ 
    // fail
    return -1;
}

int32_t open (const uint8_t* filename){
    /* The open system call provides access to the file system. The call should find the directory entry corresponding to the
    named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
    RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1. */
    pcb_t * pcb = get_cur_pcb();
    int32_t i;
    dentry_t dentry;
    if (filename == NULL){
        return -1;
    }
    int error;
    // read into dentry
    error = read_dentry_by_name(filename, &dentry);
    if (error == -1)
        return -1;

    for(i = FD_START_INDEX; i < FD_END; i++) {
        if (pcb->fd_array[i].flags == FREE) {
            pcb->fd_array[i].file_position = ; // file start position ??
            pcb->fd_array[i].flags == IN_USE; // if it is not in use, turn it to in use
            pcb->fd_array[i].inode = dentry.INodeNum;
            if (dentry.fileType == 0) { // rtc
                pcb->fd_array[i].jump_table = &rtc_op; // ??
            }
    
            else if (dentry.fileType == 1) {// directory
                pcb->fd_array[i].jump_table = &dir_op; // ??
            }
        
            else if (dentry.fileType == 2) {// file
                pcb->fd_array[i].jump_table = &file_op; // ??
            }
            return i;
        }
    }
    
    

    // fail
    return -1;
}

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
    pcb->fd_array[fd].inode = 0;
    pcb->fd_array[fd].flags = FREE;
    pcb->fd_array[fd].file_position = 0;

    return pcb->fd_array[fd].jump_table->close(fd);
}


void fileop_init(){
    stdin_fileop.close = terminal_close; 
    stdin_fileop.open = terminal_open; 
    stdin_fileop.read = terminal_read; 
    stdin_fileop.write = read_fail; 

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




// gets address to pcb corresponding to the id
pcb_t * get_pcb(int32_t id){
	uint32_t addr = EIGHTMB - EIGHTKB * (id + 1);
	return (pcb_t * )addr;
}

// gets address to current pcb
pcb_t * get_cur_pcb() {
	uint32_t addr = EIGHTMB - EIGHTKB * (curr_id + 1);
	return (pcb_t *) addr;
}

