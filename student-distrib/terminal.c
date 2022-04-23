
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
    terminal_buffer[terminal_flag][0] = '\0';

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
    clear(); 
    printf(" Switching from Terminal %d to Terminal %d \n",prevTerminal + 1,terminal_flag + 1); 

    //Save the previous terminal to its video page

    // Check if its the first time that you are opening the terminal, if so boot shell 

    // If not get its video page and move it to actual video memory 

}
