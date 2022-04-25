
#include "terminal.h"

int terminal_flag;

    // extern char buffer[BUFFER_SIZE];
    // extern int index;
    // extern int enter_detected;

    /*
    DESCRIPTION: terminal_open function for terminal driver
    INPUTS: const uint8_t *filename - pointer to the filename that we want to use
    OUTPUTS: none
    RETURN VALUE: 0 if success, -1 if fail
    SIDE EFFECTS: none
    */
    int32_t terminal_open(const uint8_t *filename)
{
    terminal_buffer[0][0] = '\0';
    terminal_buffer[1][0] = '\0';
    terminal_buffer[2][0] = '\0';

   // terminal_flag = 0;

    return 0;
}
/*
DESCRIPTION: terminal_close function for terminal driver
INPUTS: int32_t fd - 
OUTPUTS: none
RETURN VALUE: 0 if success, -1 if fail
SIDE EFFECTS: none
*/
int32_t terminal_close(int32_t fd) {
    
    return 0;
}

/*
DESCRIPTION: terminal_read for terminal driver; reads data from keyboard and stores into buffer
INPUTS: int32_t fd - file descriptor for terminal_read
        void *buf - reads data into buffer
        int32_t nbytes - number of bytes to read 
OUTPUTS: none
RETURN VALUE: number of bytes read
SIDE EFFECTS: 
*/
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes) {

    if (buf == NULL){
        return -1;
    }
    // sti(); 
    char byte;
    int bytes_read = 0;

    while (1) {
        
        if (enter_detected == 1) {

            if (nbytes > BUFFER_SIZE){
                nbytes = BUFFER_SIZE;
            }

            

            for (terminal_index = 0; terminal_index < nbytes; terminal_index++) {

                // get data from terminal buffer
                ((char *)buf)[terminal_index] = terminal_buffer[terminal_flag][terminal_index];
                bytes_read++;

                // for cleaner code
                byte = ((char * ) buf)[terminal_index];
                

                // if enter (\n) is read, finish
                if (byte == '\n') {
                    bytes_read = terminal_index + 1; 
                    break;
                } 
                
                else { // edge case: already read nbytes, adding new line
                    if (terminal_index < BUFFER_SIZE){
                        if (terminal_index == nbytes - 1 || byte == '\0'){
                            ((char * ) buf)[terminal_index] = '\n';
                            bytes_read = terminal_index + 1; 
                            break;
                        }
                    }
                }
            }
            // clear terminal buffer
            for (terminal_index = 0; terminal_index < BUFFER_SIZE; terminal_index++) {
                terminal_buffer[terminal_flag][terminal_index] = '\0';
            }

            enter_detected = 0;
            // puts(buffer);
            // printf("%u\n", terminal_index);
            // return terminal_index;
            break;
        }

    }
    //printf("%u\n", bytes_read);
    return bytes_read;
}

/*
DESCRIPTION: terminal_write for terminal driver; writes the data from the terminal_buffer
INPUTS: int32_t fd - file descriptor for terminal_write
        const void *buf - buffer to hold data
        int32_t nbytes - number of bytes to read
OUTPUTS: none
RETURN VALUE: number of bytes read
SIDE EFFECTS: writes data to screen
*/
int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes) {
    char byte;

    if (buf == NULL){
        return -1;
    }
    
    //cli();
    for (terminal_index = 0; (terminal_index < nbytes) ; terminal_index++) {
        // void can be anything
        byte = ((char * ) buf)[terminal_index];

        // printf(byte);
        // putc(byte); 

        // dpn't print NULL
        if (byte != '\0'){
            putc(byte); // print
        }
    }
    
    
    //sti();
    return nbytes;
}

extern void switch_terminals(int32_t prevTerminal)
{
    // Store page is where you are copying video memory too(Save what is in video memory to store page)
    void * storePage = (void *)(VID_ADDR + ((1+prevTerminal) * FOURKB)); 
    //Movepage is the videomemory you are moving into video memory 
    void * movePage = ( (void*)(VID_ADDR + (1+terminal_flag) * FOURKB)) ; 

    // Move the 4kb Vid memory into a seperate page to preserve it 
    memcpy(storePage,(void *)VID_ADDR,FOURKB);
    // clear(); 

     printf(" Switching from Terminal %d to Terminal %d \n", prevTerminal + 1, terminal_flag + 1); 
    // Check if you have opened this terminal if not, execute shell 
    memcpy((void *)VID_ADDR,movePage,FOURKB); 
    if(terminals[terminal_flag].shellRunning == 0){
        terminals[terminal_flag].shellRunning = 1 ;
        execute((const uint8_t *)"shell"); 
    }



    // uint8_t eip_value[SIZE_OF_INT32];
    // read_data(dentry.INodeNum, STARTEXEC, eip_value, SIZE_OF_INT32);

    // // Set stack pointer to the bottom of the 4 MB page
    // esp_usr = PAGE_SIZE - SIZE_OF_INT32 + USER_V;
    // eip_usr = * ((int * )eip_value);

    // // TSS initalization
    // pcb->active = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * curr_id);
    // tss.esp0 = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * curr_id);
    // tss.ss0 = KERNEL_DS;
    


    // // Save old EBP and ESP
    // register uint32_t saved_ebp asm("ebp");
    // register uint32_t saved_esp asm("esp");
    // pcb->save_ebp = saved_ebp;
    // pcb->save_esp = saved_esp;
    // pcb->usr_eip = eip_usr;
    // pcb->usr_esp = esp_usr;

    // // =============================== Push IRET context to kernel stack  ===============================
    // // Set the registers that we want to pop to the correct values
    // // Possible reasons it doesnt work: segment registers need to be set
    // sti();

    // // Push an iret stack to switch to our user program in the following order
    // // USER_DS, ESP_USR, EFLAGS, USER_CS, EIP
    // //Also set the value of DS(Might happen when iret is called but we did it to be safe )
    // asm volatile (
    //     "andl $0x00FF, %%edx \n "
    //     "movw %%dx,%%ds \n "
    //     "pushl %%edx \n"
    //     "pushl %%ecx \n" 
    //     "pushfl      \n"
    //     "popl %%ecx  \n"
    //     "orl $0x0200, %%ecx   \n"
    //     "pushl %%ecx \n"
    //     "pushl %%ebx \n"
    //     "pushl %%eax \n"
    //     "iret \n"
    //     :
    //     : "a"(eip_usr), "b"(USER_CS), "c"(esp_usr), "d"(USER_DS)
    //     : "memory" 
    // );
    // //Label for halt to jump to after 
    // asm volatile("leaveExec: \n");

    // // Move the return value from halt(status) into a value we can return in C
    // asm volatile("movl %%eax, %%ebx \n"
    // :   "=b"(returnVal)
    // : 
    // : "memory"
    // );


}
