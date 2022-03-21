#include "keyboard.h"


//holds the values corresponding to the keycodes to display to screen
char keycode_to_char[58] = {0, 0, '1', '2', '3', '4', 
'5', '6', '7', '8', '9', '0', '-', '=', 0, ' ', 'q', 'w', 'e', 'r', 't', 'y', 
'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 
'l', ';', '\'', '`', 0,  0, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
0, 0, 0, 0};
/*
DESCRIPTION: initializes the keyboard for interrupts
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: sets IRQ1 to keyboard interrupts
*/
void keyboard_init(void) {
    enable_irq(KEYBOARD_IRQ);   //enables the irq for keyboard (irq num is 1 according to google)
}
/*
DESCRIPTION: handles the interrupts for keyboard
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: Displays the characters that are pressed on the keyboard to the screen
*/
extern void keyboard_handler(void) { 
    cli();
    uint32_t keycode = inb(KEYBOARD_DATA_PORT);
    if(keycode >= SPECIAL_KEYCODES) {                           //for now, we won't handle keycodes after 0x81
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    putc(keycode_to_char[keycode]);
    send_eoi(KEYBOARD_IRQ);
    sti();
}
