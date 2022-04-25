#ifndef TERMINAL_H
#define TERMINAL_H

#include "keyboard.h"
#include "lib.h"
#include "types.h"
#include "systemcalls.h"


typedef struct terminal_data{
    int shellRunning; 
    int screen_x; 
    int screen_y; 
    int currPID; 

}terminal_data_t; 

int32_t terminal_open(const uint8_t *filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes);
extern void switch_terminals(int32_t prevTerminal);
terminal_data_t terminals[3]; 

#endif


