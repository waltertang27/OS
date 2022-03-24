#include "keyboard.h"

//holds the values corresponding to the keycodes to display to screen
/*
char keycode_to_char[58] = {0, 0, '1', '2', '3', '4', 
'5', '6', '7', '8', '9', '0', '-', '=', 0, ' ', 'q', 'w', 'e', 'r', 't', 'y', 
'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 
'l', ';', '\'', '`', 0,  0, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
0, 0, 0, 0};
*/

//represents the index in keycode_to_char, when it is the original or (shifted) version of the key
#define INDEX0 0
#define INDEX1 1

int CAPS_ON = 0;

int shift_flag = 0;
int ctrl_flag = 0;
int capslock_flag = 0;
int alt_flag = 0;

char keycode_to_char[91][2] = {
{0, 0}, //0x0, empty
{0, 0}, //escape
{'1', '!'}, //0x02
{'2', '@'}, //0x03
{'3', '#'}, //0x04
{'4', '$'}, //0x05
{'5', '%'}, //0x06
{'6', '^'}, //0x07
{'7', '&'}, //0x08
{'8', '*'}, //0x09
{'9', '('}, //0x0A
{'0', ')'}, //0x0B
{'-', '_'}, //0x0C
{'=', '+'}, //0x0D
{}, //0x0E, backspace
{0, 0}, //0x0F, tab
{'q', 'Q'}, //0x10
{'w', 'W'}, //0x11
{'e', 'E'}, 
{'r', 'R'}, 
{'t', 'T'}, 
{'y', 'Y'}, 
{'u', 'U'}, 
{'i', 'I'}, 
{'o', 'O'}, 
{'p', 'P'}, 
{'[', '{'}, 
{']', '}'}, //0x1B
{'\n', '\n'}, //enter 
{0, 0}, //LEFT CONTROL, 0x1D 
{'a', 'A'}, 
{'s', 'S'}, 
{'d', 'D'}, 
{'f', 'F'}, 
{'g', 'G'}, 
{'h', 'H'}, 
{'j', 'J'}, 
{'k', 'K'}, 
{'l', 'L'}, 
{';', ':'}, 
{'\'', '"'}, //0x28
{'`', '~'}, 
{0, 0}, //LEFT SHIFT, 0x2A 
{'\\', '|'}, //0x2B
{'z', 'Z'}, 
{'x', 'X'}, 
{'c', 'C'}, 
{'v', 'V'}, 
{'b', 'B'}, 
{'n', 'N'}, 
{'m', 'M'}, 
{',', '<'}, 
{'.', '>'},
{'/', '?'}, //0x35
{0, 0}, //RIGHT SHIFT, 0x36 
{0, 0}, //0x37 ?? (keypad) * pressed
{0, 0}, //LEFT ALT, 0x38 
{' ', ' '}, //SPACE, 0x39 
{0, 0}, //CAPS LOCK, 0x3A 
/*FUNCTION KEYS F1 to F10*/
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0},  //NUM_LOCK
{0, 0}, //SCROLL_LOCK, 0x46
/* NUMBER PAD */
{0, 0},
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0}, 
{0, 0},
/*0x54 to 0x56 is empty*/
{0, 0}, 
{0, 0}, 
{0, 0},   
{0, 0}, //F11
{0, 0} //F12 0x58
};

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

    if(keycode == CAPSLOCK_PRESSED && CAPS_ON == 0) {
        capslock_flag = 1;
        CAPS_ON = 1;
    }
    else if(keycode == CAPSLOCK_PRESSED && CAPS_ON == 1) {
        capslock_flag = 0;
        CAPS_ON = 0;
    }
    if(keycode == LSHIFT_PRESSED) {
        shift_flag = 1;
    }
    if(keycode == LSHIFT_RELEASED) {
        shift_flag = 0;
    }
    if(keycode == RSHIFT_PRESSED) {
        shift_flag = 1;
    }
    if(keycode == RSHIFT_RELEASED) {
        shift_flag = 0;
    }
    if(keycode == LALT_PRESSED) {
        alt_flag = 1;
    }
    if(keycode == LALT_RELEASED) {
        alt_flag = 0;
    }

    if(keycode >= SPECIAL_KEYCODES) {                           //for now, we won't handle keycodes after 0x81
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    //prints for tab
    if(keycode == 0x0F) {
        printf("%s", "    ");
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    if(capslock_flag == 1) {
        char temp = keycode_to_char[keycode][INDEX0];
        //checks if it is a letter, if so, display the uppercase version
        if(temp >= 'a' && temp <= 'z') {
            printf("%c", keycode_to_char[keycode][INDEX1]);
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
        //otherwise, display original
        else {
            printf("%c", keycode_to_char[keycode][INDEX0]);
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
    }
    //if shift is held
    if(shift_flag == 1) {
        printf("%c", keycode_to_char[keycode][INDEX1]);
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    printf("%c", keycode_to_char[keycode][INDEX0]);
    send_eoi(KEYBOARD_IRQ);
    sti();
    return;
}
