#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "types.h"

#define KEYBOARD_IRQ  1

#define KEYBOARD_DATA_PORT   0x60

#define SPECIAL_KEYCODES    0x81

#define CAPSLOCK_PRESSED 0x3A
#define CAPSLOCK_RELEASED 0xBA

#define LSHIFT_PRESSED 0x2A
#define LSHIFT_RELEASED 0xAA

#define RSHIFT_PRESSED 0x36
#define RSHIFT_RELEASED 0xB6

#define LALT_PRESSED 0x38
#define LALT_RELEASED 0xB8

#define LCTRL_PRESSED 0x1D
#define LCTRL_RELEASED 0x9D

#define ENTER 0x1C
#define BACKSPACE 0x0E
#define L_KEY 0x26
#define TAB 0x0F

#define F1 0x3B
#define F2 0x3C
#define F3 0x3D

//represents the index in keycode_to_char, when it is the original or (shifted) version of the key
#define INDEX0 0
#define INDEX1 1

#define BUFFER_SIZE 128 //maximum size of the char buffer
/*
#define RALT_PRESSED 0xE0
#define RALT_RELEASED 
*/

extern char enter_buffer[BUFFER_SIZE];

extern char buffer[BUFFER_SIZE];   //buffer to hold what to display to print
extern char terminal_buffer[3][BUFFER_SIZE];
extern int terminal_index;
extern int terminal_flag
extern int clear_buffer;

extern int index;     //index of what we want to display till
extern int enter_index;
//buffer[index] = '\0';
extern int enter_detected;

extern int backspace_detected;
extern int ctrl_l_detected;
 

void keyboard_init(void);

extern void keyboard_handler(void);



#endif 

