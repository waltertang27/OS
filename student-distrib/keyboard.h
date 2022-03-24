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
/*
#define RALT_PRESSED 0xE0
#define RALT_RELEASED 
*/

void keyboard_init(void);

extern void keyboard_handler(void);

#endif 

