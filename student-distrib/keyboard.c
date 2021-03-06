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
extern int32_t curr_id; 

int terminal_flag;
int terminal_shell[3]; 

int prevTerminal;


/*
extern char buffer[BUFFER_SIZE];   //buffer to hold what to display to print
extern int index;     //index of what we want to display till
//buffer[index] = '\0';
extern int enter_detected;
*/


char buffer[BUFFER_SIZE];



char terminal_buffer[3][BUFFER_SIZE];
char second_line_buffer[BUFFER_SIZE];
int terminal_index;
char buf[1046]; 


int index[3];
int index_flag;



int enter_detected;
int backspace_detected;




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
    index[0] = 0;
    index[1] = 0;
    index[2] = 0;
    index_flag = 0;
    terminal_index = 0;
    enter_detected = 0;
    backspace_detected = 0;



    terminal_flag = 0;
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
    
    page_table[VIDEO_PAGE_INDEX].page_table_addr = 184; 

    uint32_t keycode = inb(KEYBOARD_DATA_PORT);

    if(keycode == CAPSLOCK_PRESSED && CAPS_ON == 0) {
        capslock_flag = 1;
        CAPS_ON = 1;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    else if(keycode == CAPSLOCK_PRESSED && CAPS_ON == 1) {
        capslock_flag = 0;
        CAPS_ON = 0;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode == LSHIFT_PRESSED) {
      //  printf("CurrID: %d",curr_id);
        shift_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode == LSHIFT_RELEASED) {
        shift_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode == RSHIFT_PRESSED) {
        shift_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode == RSHIFT_RELEASED) {
        shift_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode == LALT_PRESSED) {
        alt_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode == LALT_RELEASED) {
        alt_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode == LCTRL_PRESSED) {
        ctrl_flag = 1;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode == LCTRL_RELEASED) {
        ctrl_flag = 0;
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    if(keycode >= SPECIAL_KEYCODES) {                           //for now, we won't handle keycodes after 0x81
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    if(alt_flag == 1) {

        if(keycode == F1 && terminal_flag != 0) {

            prevTerminal = terminal_flag; 
            terminal_flag = 0;
            index_flag = 0;

            memcpy(buffer, terminal_buffer[terminal_flag], strlen(terminal_buffer[terminal_flag]) + 1);
            
            send_eoi(KEYBOARD_IRQ);
            sti();
            switch_terminals(prevTerminal);
            //puts(buffer);
            return;
        }
        else if(keycode == F2 && terminal_flag != 1) {

            prevTerminal = terminal_flag;
            terminal_flag = 1;
            index_flag = 1;
          
            memcpy(buffer, terminal_buffer[terminal_flag], strlen(terminal_buffer[terminal_flag]) + 1);
           
            send_eoi(KEYBOARD_IRQ);
            sti();
            switch_terminals(prevTerminal);
            //puts(buffer);
        
            return;
        }
        else if(keycode == F3 && terminal_flag != 2) {

            prevTerminal = terminal_flag;
            terminal_flag = 2;
            index_flag = 2;

            memcpy(buffer, terminal_buffer[terminal_flag], strlen(terminal_buffer[terminal_flag]) + 1);

            send_eoi(KEYBOARD_IRQ);
            sti();
            switch_terminals(prevTerminal);
            //puts(buffer);
            
            return;
        }
        else {
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;

        }
        
    }


    if(index[index_flag] >= BUFFER_SIZE - 2) {

        
        if(keycode == ENTER) {
            enter_detected = 1;

            terminal_buffer[terminal_flag][index[index_flag]] = '\n';
            terminal_buffer[terminal_flag][index[index_flag] + 1] = '\0';
            buffer[index[index_flag]] = '\n';
            buffer[index[index_flag] + 1] = '\0';
            putc(buffer[index[index_flag]]);

            index[index_flag] = 0;
            buffer[index[index_flag]] = '\0';


            send_eoi(KEYBOARD_IRQ);
  
            sti();
            return;
        }
        else if(keycode == BACKSPACE) {
            backspace_detected = 1;
            buffer[index[index_flag] - 1] = '\0';
            terminal_buffer[terminal_flag][index[index_flag] - 1] = '\0';
            putc(buffer[index[index_flag] - 2]);
            index[index_flag] = index[index_flag] - 1;
            
        }
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }

    if(keycode == ENTER) {
        enter_detected = 1;
        buffer[index[index_flag]] = '\n';
        buffer[index[index_flag] + 1] = '\0';

        terminal_buffer[terminal_flag][index[index_flag]] = '\n';
        terminal_buffer[terminal_flag][index[index_flag] + 1] = '\0';

    
        putc(buffer[index[index_flag]]);


        index[index_flag] = 0;
       
        send_eoi(KEYBOARD_IRQ);
        sti();
        
        return;
    }
    //0x26 is the value L where ctrl L clears the buffer
    if(ctrl_flag == 1 && keycode == L_KEY) {

        buffer[0] = '\0';
        terminal_buffer[terminal_flag][0] = '\0';
      
        index[index_flag] = 0;
        clear();
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    //checks for backspace
    if(keycode == BACKSPACE) {
       

        if(index[index_flag] == 0) {

            
            buffer[index[index_flag]] = '\0';
            terminal_buffer[terminal_flag][index[index_flag]] = '\0';
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
       
        if(index > 0) {
            
            backspace_detected = 1;
            
            buffer[index[index_flag] - 1] = '\0';

            terminal_buffer[terminal_flag][index[index_flag] - 1] = '\0';
            
            
            putc(buffer[index[index_flag] - 2]);

            index[index_flag] = index[index_flag] - 1;

            
           
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
    
    }


    if(keycode == TAB) {
        
        buffer[index[index_flag]] = '\t';
        buffer[index[index_flag] + 1] = '\0';
        terminal_buffer[terminal_flag][index[index_flag]] = '\t';
        terminal_buffer[terminal_flag][index[index_flag] + 1] = '\0';

  
        putc(buffer[index[index_flag]]);

        index[index_flag] = index[index_flag] + 1;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }

    if(capslock_flag == 1) {
        char temp = keycode_to_char[keycode][INDEX0];
        if(shift_flag == 1) {
            char temp = keycode_to_char[keycode][INDEX0];
            if(temp >= 'a' && temp <= 'z') {
                terminal_buffer[terminal_flag][index[index_flag]] = keycode_to_char[keycode][INDEX0];
                terminal_buffer[terminal_flag][index[index_flag] + 1] = '\0';

                buffer[index[index_flag]] = keycode_to_char[keycode][INDEX0];
                buffer[index[index_flag] + 1] = '\0';
                
        
                putc(buffer[index[index_flag]]);
                index[index_flag] = index[index_flag] + 1;
                send_eoi(KEYBOARD_IRQ);
                sti();
                return;
            }
        }
        //checks if it is a letter, if so, display the uppercase version
        if(temp >= 'a' && temp <= 'z') {
            
            terminal_buffer[terminal_flag][index[index_flag]] = keycode_to_char[keycode][INDEX1];
            terminal_buffer[terminal_flag][index[index_flag] + 1] = '\0';

            buffer[index[index_flag]] = keycode_to_char[keycode][INDEX1];
            buffer[index[index_flag] + 1] = '\0';
           
            putc(buffer[index[index_flag]]);
            index[index_flag] = index[index_flag] + 1;
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
        //otherwise, display original
        else {
            terminal_buffer[terminal_flag][index[index_flag]] = keycode_to_char[keycode][INDEX0];
            terminal_buffer[terminal_flag][index[index_flag] + 1] = '\0';
            
            buffer[index[index_flag]] = keycode_to_char[keycode][INDEX0];
            buffer[index[index_flag] + 1] = '\0';
         
            putc(buffer[index[index_flag]]);
            index[index_flag] = index[index_flag] + 1;
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
    }
    //if shift is held
    if(shift_flag == 1) {
        terminal_buffer[terminal_flag][index[index_flag]] = keycode_to_char[keycode][INDEX1];
        terminal_buffer[terminal_flag][index[index_flag] + 1] = '\0';

        buffer[index[index_flag]] = keycode_to_char[keycode][INDEX1];
        buffer[index[index_flag] + 1] = '\0';
       
        putc(buffer[index[index_flag]]);
        index[index_flag] = index[index_flag] + 1;

        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }

    

    
    terminal_buffer[terminal_flag][index[index_flag]] = keycode_to_char[keycode][INDEX0];
   


   
    buffer[index[index_flag]] = keycode_to_char[keycode][INDEX0];
    buffer[index[index_flag] + 1] = '\0';
 
   

    putc(buffer[index[index_flag]]);
   
    index[index_flag] = index[index_flag] + 1;
    send_eoi(KEYBOARD_IRQ);
    sti();
    return;
}
 

