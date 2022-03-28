
#include "terminal.h"


char terminal_buffer[BUFFER_SIZE];
int terminal_index = 0;

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
    while(1) {
        if(enter_detected == 1) {
            puts(buffer);
            memcpy(terminal_buffer, enter_buffer, strlen(enter_buffer) + 1);
            terminal_index = enter_index;
            break;
        }
    }
    enter_detected = 0;
    //puts(buffer);
    //printf("%u\n", terminal_index);
    return terminal_index;

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
    //puts("write started\n");
    puts(terminal_buffer);
    terminal_index = 0;
    terminal_buffer[terminal_index] = '\0';
    return terminal_index;
}



