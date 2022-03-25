
#include "terminal.h"


char* terminal_buffer;
int terminal_index = 0;

//extern char buffer[BUFFER_SIZE];
extern int index;
extern int enter_detected;


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
            terminal_buffer = get_buffer();
            terminal_index = index;
            break;
        }
    }
    return terminal_index;

}

int32_t terminal_write() {
    puts(terminal_buffer);
    terminal_index = 0;
    terminal_buffer[terminal_index] = '\0';
    return terminal_index;
}



