
#include "terminal.h"


char terminal_buffer[BUFFER_SIZE];
int terminal_index = 0;

//extern char buffer[BUFFER_SIZE];
//extern int index;
//extern int enter_detected;


int32_t terminal_open() {
    terminal_buffer[0] = '\0';

    return 0;
}

int32_t terminal_close() {
    
    return 0;
}


int32_t terminal_read() {
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

int32_t terminal_write() {
    //puts("write started\n");
    puts(terminal_buffer);
    terminal_index = 0;
    terminal_buffer[terminal_index] = '\0';
    return terminal_index;
}



