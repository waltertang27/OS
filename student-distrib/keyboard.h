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


void keyboard_init(void);

extern void keyboard_handler(void);

#endif 

