#include "keyboard.h"
#include "types.h"

//holds the values corresponding to the keycodes to display to screen
/*
char keycode_to_char[58] = {0, 0, '1', '2', '3', '4', 
'5', '6', '7', '8', '9', '0', '-', '=', 0, ' ', 'q', 'w', 'e', 'r', 't', 'y', 
'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 
'l', ';', '\'', '`', 0,  0, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
0, 0, 0, 0};
*/



int CAPS_ON = 0;
int shift_flag = 0;
int ctrl_flag = 0;
int capslock_flag = 0;
int alt_flag = 0;
/*
extern char buffer[BUFFER_SIZE];   //buffer to hold what to display to print
extern int index;     //index of what we want to display till
//buffer[index] = '\0';
extern int enter_detected;
*/


char buffer[BUFFER_SIZE];
char enter_buffer[BUFFER_SIZE];
char terminal_buffer[BUFFER_SIZE];
char second_line_buffer[BUFFER_SIZE];
int terminal_index;

int clear_buffer;

int index;
int enter_index;
int enter_detected;
int backspace_detected;
int ctrl_l_detected;



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
{'\b', '\b'}, //0x0E, backspace
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
    buffer[0] = '\0';
    second_line_buffer[0] = '\0';
    index = 0;
    terminal_index = 0;
    enter_detected = 0;
    backspace_detected = 0;
    ctrl_l_detected = 0;
    clear_buffer = 0;
    //enable_cursor();
    //update_cursor();
}
/*
DESCRIPTION: handles the interrupts for keyboard. stores the pressed key values into a buffer. 
INPUTS: none
OUTPUTS: uses the functions in lib.c (puts) to print to screen. 
RETURN VALUE: none
SIDE EFFECTS: Displays the characters that are pressed on the keyboard to the screen
*/
extern void keyboard_handler(void) {

    //int loop;
    cli();
    uint32_t keycode = inb(KEYBOARD_DATA_PORT);

    // if (clear_buffer == 1){
    //     memcpy(enter_buffer, buffer, strlen(buffer) + 1);

    //     for (loop = 0; loop < BUFFER_SIZE; loop++){
    //         buffer[loop] = '\0';
    //         index = 0;
    //     }
    //     clear_buffer = 0;
    // }


    if(keycode == CAPSLOCK_PRESSED && CAPS_ON == 0) {
        capslock_flag = 1;
        CAPS_ON = 1;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    else if(keycode == CAPSLOCK_PRESSED && CAPS_ON == 1) {
        capslock_flag = 0;
        CAPS_ON = 0;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode == LSHIFT_PRESSED) {
        shift_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode == LSHIFT_RELEASED) {
        shift_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode == RSHIFT_PRESSED) {
        shift_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode == RSHIFT_RELEASED) {
        shift_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode == LALT_PRESSED) {
        alt_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode == LALT_RELEASED) {
        alt_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode == LCTRL_PRESSED) {
        ctrl_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode == LCTRL_RELEASED) {
        ctrl_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        cli();
        return;
    }
    if(keycode >= SPECIAL_KEYCODES) {                           //for now, we won't handle keycodes after 0x81
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    if(index >= BUFFER_SIZE) {
        
        if(keycode == ENTER) {
            buffer[0] = '\0';
            index = 0;
            putc('\n');
        }
        //else {
            //printf("%s", "char buffer is full!\n");
        //}
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }

    if(keycode == ENTER) {
        enter_detected = 1;
        buffer[index] = '\n';
        buffer[index + 1] = '\0';
        if (index >= 80){
            second_line_buffer[index - 80] = '\n';
            second_line_buffer[index - 80 + 1] = '\0';
            //puts(second_line_buffer);
            putc(second_line_buffer[index - 80]);
        } else {
            //puts(buffer);
            putc(buffer[index]);
        }


        index = index + 1;
        send_eoi(KEYBOARD_IRQ);
        //a newline clears the buffer
        enter_index = index;
        memcpy(enter_buffer, buffer, strlen(buffer) + 1);


        index = 0;
        buffer[index] = '\0';
        second_line_buffer[index] = '\n';
        sti();
        return;
    }
    //0x26 is the value L where ctrl L clears the buffer
    if(ctrl_flag == 1 && keycode == L_KEY) {
        ctrl_l_detected = 1;
        buffer[0] = '\0';
        //for(i = 0; i < index; i++) {
        //    buffer[i] = ' ';
        //}
        index = 0;
        //puts(buffer);
        clear();
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    //checks for backspace
    if(keycode == BACKSPACE) {
        if(index == 0) {
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
        if(index == 1) {
            //buffer[0] = '\b';
            buffer[1] = '\b';
            buffer[2] = '\0';
            index--;
            putc(buffer[1]);
            buffer[1] = '\0';
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
        if(index > 0) {
            int count = 3;
            backspace_detected = 1;
            //if we want to delete a tab
            if(buffer[index - 1] == '\t') {
                while(count > 0 && index > 1) {
                    if(buffer[index - 1] == '\t') {
                        index = index - 1;
                        count--;
                    }
                    else {
                        count = 0;
                    }
                }
            }
            /*
            if(index == 1) {
                buffer[index - 1] = '\0';
                buffer[index] = '\0';
                index = index - 1;
                //putc(' ');
                puts(buffer);
                send_eoi(KEYBOARD_IRQ);
                sti();
                return;
            }
            */
           // if(index == 0) {
           //     buffer[index] = '\0'
           // }
            terminal_buffer[index] = '\b';
            terminal_buffer[index + 1] = '\0';
            buffer[index] = '\b';
            buffer[index + 1] = '\0';
            backspace_detected = 1;
            //buffer[index - 1] = '\0;
            if (index >= 80){
                second_line_buffer[index - 80 + 1] = '\b';
                second_line_buffer[index - 80 + 1 + 1] = '\0';
                // puts(second_line_buffer);
                putc(second_line_buffer[index - 80 + 1]);
                //second_line_buffer[index - 80 + 1] = '\0';
            } else {
                //puts(buffer);
                putc(buffer[index]);
                //buffer[index] = '\0';
            }

            index = index - 1;

            
            //puts(buffer);

            //buffer[index - 1] = '\0';
            //index = index - 1;
            //puts(buffer);
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
        else {
            //puts(buffer);
            putc(buffer[index + 1]);
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
    }


    if(keycode == TAB) {
        //printf("%s", "    ");
        //adjusts space correctly based on if index might go out of bounds; BUFFER_SIZE - NUM where num is the number of spaces left remaining in buffer
        //not out of bounds
        /*
        if(index <= BUFFER_SIZE - 5) {
            buffer[index] = '\t';
            buffer[index + 1] = '\t';
            buffer[index + 2] = '\t';
            buffer[index + 3] = '\t';  
            buffer[index + 4] = '\0';
            index = index + 4;
        }
        //3 spaces left in buffer
        else if(index == BUFFER_SIZE - 4) {
            buffer[index] = '\t';
            buffer[index + 1] = '\t';
            buffer[index + 2] = '\t';
            buffer[index + 3] = '\0';
            index = index + 3;
        }
        //2 spaces left in buffer
        else if(index == BUFFER_SIZE - 3) {
            buffer[index] = '\t';
            buffer[index + 1] = '\t';
            buffer[index + 2] = '\0';
            index = index + 2;
        }
        //1 space left in buffer
        else if(index == BUFFER_SIZE - 2) {
            buffer[index] = '\t';
            buffer[index + 1] = '\0';
            index = index + 1;
        } */
        buffer[index] = '\t';
        buffer[index + 1] = '\0';
        terminal_buffer[index] = '\t';
        terminal_buffer[index + 1] = '\0';
        index++;
        //puts(buffer);
        putc(buffer[index - 1]);
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    if(capslock_flag == 1 && index <= BUFFER_SIZE - 2) {
        char temp = keycode_to_char[keycode][INDEX0];
        if(shift_flag == 1) {
            char temp = keycode_to_char[keycode][INDEX0];
            if(temp >= 'a' && temp <= 'z') {
                terminal_buffer[index] = keycode_to_char[keycode][INDEX0];
                buffer[index] = keycode_to_char[keycode][INDEX0];
                buffer[index + 1] = '\0';
                index = index + 1;
                if (index >= 80){
                    second_line_buffer[index - 80] = keycode_to_char[keycode][INDEX0];
                    second_line_buffer[index - 80 + 1] = '\0';
                    //puts(second_line_buffer);
                    putc(second_line_buffer[index - 80]);
                } else {
                    //puts(buffer);
                    putc(buffer[index - 1]);
                }
                send_eoi(KEYBOARD_IRQ);
                sti();
                return;
            }
        }
        //checks if it is a letter, if so, display the uppercase version
        if(temp >= 'a' && temp <= 'z') {
            //printf("%c", keycode_to_char[keycode][INDEX1]);
            terminal_buffer[index] = keycode_to_char[keycode][INDEX1];
            buffer[index] = keycode_to_char[keycode][INDEX1];
            buffer[index + 1] = '\0';
            index = index + 1;
            if (index >= 80){
                second_line_buffer[index - 80] = keycode_to_char[keycode][INDEX1];
                second_line_buffer[index - 80 + 1] = '\0';
                //puts(second_line_buffer);
                putc(second_line_buffer[index - 80]);
            } else {
                //puts(buffer);
                putc(buffer[index - 1]);
            }
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
        //otherwise, display original
        else {
            terminal_buffer[index] = keycode_to_char[keycode][INDEX0];
            //printf("%c", keycode_to_char[keycode][INDEX0]);
            buffer[index] = keycode_to_char[keycode][INDEX0];
            buffer[index + 1] = '\0';
            index = index + 1;
            if (index >= 80){
                second_line_buffer[index - 80] = keycode_to_char[keycode][INDEX0];
                second_line_buffer[index - 80 + 1] = '\0';
                //puts(second_line_buffer);
                putc(second_line_buffer[index - 80]);
            } else {
                //puts(buffer);
                putc(buffer[index - 1]);
            }
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
    }
    //if shift is held
    if(shift_flag == 1 && index <= BUFFER_SIZE - 2) {
        terminal_buffer[index] = keycode_to_char[keycode][INDEX1];
        
        //printf("%c", keycode_to_char[keycode][INDEX1]);
        buffer[index] = keycode_to_char[keycode][INDEX1];
        buffer[index + 1] = '\0';
        index = index + 1;
        if (index >= 80){
            second_line_buffer[index - 80] = keycode_to_char[keycode][INDEX1];
            second_line_buffer[index - 80 + 1] = '\0';
            //puts(second_line_buffer);
            putc(second_line_buffer[index - 80]);
        } else {
           // puts(buffer);
           putc(buffer[index - 1]);
        }
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }

    

    //printf("%c", keycode_to_char[keycode][INDEX0]);
    terminal_buffer[index] = keycode_to_char[keycode][INDEX0];
    buffer[index] = keycode_to_char[keycode][INDEX0];
    buffer[index + 1] = '\0';
    index = index + 1;
    if (index >= 80){
        second_line_buffer[index - 80] = keycode_to_char[keycode][INDEX0];
        second_line_buffer[index - 80 + 1] = '\0';
        //puts(second_line_buffer);
        putc(second_line_buffer[index - 80]);
    } else {
        //puts(buffer);
        putc(buffer[index - 1]);
    }

    send_eoi(KEYBOARD_IRQ);
    sti();
    return;
}
