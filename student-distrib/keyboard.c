#include "keyboard.h"



char keycode_to_char[58] = {0, 0, '1', '2', '3', '4', 
'5', '6', '7', '8', '9', '0', '-', '=', 0, ' ', 'q', 'w', 'e', 'r', 't', 'y', 
'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 
'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
0, 0, 0, 0};

void keyboard_init(void) {

    enable_irq(KEYBOARD_IRQ);   //enables the irq for keyboard (irq num is 1 according to google)

}

extern void keyboard_handler(void) {
   // putc('z');
    cli();
    uint32_t keycode = inb(KEYBOARD_DATA_PORT);
    //putc('a');
   
    //putc(keycode + '0');
    putc(keycode_to_char[keycode]);
    //printf("%c", keycode_to_char[keycode]);
    send_eoi(KEYBOARD_IRQ);
    sti();
}
