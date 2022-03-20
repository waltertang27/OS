
#include "keyboard.h"


void keyboard_init(void) {
    enable_irq(KEYBOARD_IRQ);   //enables the irq for keyboard (irq num is 1 according to google)
}

void keyboard_handler(void) {
    cli();
    uint32_t keycode = inb(KEYBOARD_DATA_PORT);
    printf("%u\n", keycode);
    send_eoi(KEYBOARD_IRQ);
    sti();
}