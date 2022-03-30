
#include "terminal.h"




//extern char buffer[BUFFER_SIZE];
//extern int index;
//extern int enter_detected;

/*
DESCRIPTION: terminal_open function for terminal driver
INPUTS: const uint8_t *filename - pointer to the filename that we want to use
OUTPUTS: none
RETURN VALUE: 0 if success, -1 if fail
SIDE EFFECTS: none
*/
int32_t terminal_open(const uint8_t *filename) {
    terminal_buffer[0] = '\0';

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
        void *buf - reads data from a given file
        int32_t nbytes - integer to maintain size of frequency (4 bytes)
OUTPUTS: none
RETURN VALUE: number of bytes read
SIDE EFFECTS: 
*/
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes) {
    // while(1) {
    //     if(enter_detected == 1) {
    //         puts(buffer);
    //         memcpy(terminal_buffer, enter_buffer, strlen(enter_buffer) + 1);
    //         terminal_index = enter_index;
    //         break;
    //     }
    // }
    // enter_detected = 0;
    // //puts(buffer);
    // //printf("%u\n", terminal_index);
    // return terminal_index;

    if (buf == NULL){
        return -1;
    }

    char byte;
    int bytes_read = 0;

    while (1) {
        if (enter_detected == 1) {
            cli();

            if (nbytes > BUFFER_SIZE){
                nbytes = BUFFER_SIZE;
            }

            for (terminal_index = 0; terminal_index < nbytes; terminal_index++) {

                // get data from terminal buffer
                ((char * ) buf)[terminal_index] = terminal_buffer[terminal_index];
                bytes_read++;

                // for cleaner code
                byte = ((char * ) buf)[terminal_index];

                // if enter (\n) is read, finish
                if (byte == '\n') {
                    break;
                } else { // edge case: already read nbytes, adding new line
                    if (nbytes < BUFFER_SIZE){
                        if (terminal_index == nbytes - 1){
                            ((char * ) buf)[terminal_index] = '\n';
                            break;
                        }
                    }
                }
                
            }

            // clear terminal buffer
            for (terminal_index = 0; terminal_index < BUFFER_SIZE; terminal_index++) {
                terminal_buffer[terminal_index] = '\0';
            }

            enter_detected = 0;
            // puts(buffer);
            // printf("%u\n", terminal_index);
            // return terminal_index;
            sti();
            break;
        }
    }
    // printf("size: %d", bytes_read);
    return bytes_read;

}

/*
DESCRIPTION: terminal_write for terminal driver; writes the data from the terminal_buffer
INPUTS: int32_t fd - file descriptor for terminal_write
        const void *buf - writes data from a given file
        int32_t nbytes - integer to maintain size of frequency (4 bytes)
OUTPUTS: none
RETURN VALUE: number of bytes read
SIDE EFFECTS: writes data to screen
*/
int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes) {
    // puts("write started\n");
    // cli();
    // puts(terminal_buffer);
    // terminal_index = 0;
    // terminal_buffer[terminal_index] = '\0';
    // sti();
    // return terminal_index;
    char byte;

    if (buf == NULL){
        return -1;
    }
    
    cli();
    for (terminal_index = 0; terminal_index < nbytes; terminal_index++) {
        // void can be anything
        byte = ((char * ) buf)[terminal_index];

        // printf(byte);
        // putc(byte); 

        // dpn't print NULL
        if (byte != '\0'){
            putc(byte); // print
        }
    }
    sti();
    return nbytes;
}

// void set_terminal_buffer(){
//     terminal_buffer[0] = 'q';
// }

