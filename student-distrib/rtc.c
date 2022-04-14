#include "rtc.h" 

// uint32_t rtc_rate;
volatile uint32_t rtc_int;
int testing_RTC; 
/*
DESCRIPTION: Initializes RTC registers and turn on IRQ 8
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: RTC fires 
*/
extern void rtc_init(void){
    printf("RTC Started init \n");

    outb(RTC_REG_B, RTC_PORT_1); // select Status Register B, and disable NMI (by setting the 0x80 bit)
    char prev = inb(RTC_PORT_2); // read the current value of register B
    
    outb(RTC_REG_B,RTC_PORT_1); //set the index again (a read will reset the index to register D)
    outb(prev | SIXTH_BIT_MASK ,RTC_PORT_2); //write the previous value ORed with 0x40. This turns on bit 6 of register B

    outb(RTC_REG_A,RTC_PORT_1); // Do the same for register A as was done above 
    prev = inb(RTC_PORT_2);
    outb(RTC_REG_A,RTC_PORT_1);
    outb(prev & (TOP_FOUR_BITMASK | LOWER_FOUR_BITMASK) ,RTC_PORT_2); //0F allows to mask for top 4 bits
    testing_RTC = 0; 
    
    // the initial frequency is set to 2 interrupts/second;
    rtc_freq(MAX_FREQ);
    rtc_int = 0;
    enable_irq(RTC_IRQ_NUM);
}

/*
DESCRIPTION: Introduces periodic interrupt in the RTC 
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: RTC continiously fires 
*/
extern void rtc_handler(void){
    cli();
    rtc_int = 1; // interrupts occur and not handled yet
    // test_interrupts();
    outb(RTC_REG_C, RTC_PORT_1);	// select register C
    inb(RTC_PORT_2);		        // just throw away contents
    send_eoi(RTC_IRQ_NUM);
    sti();
}

/* open_rtc
 * Input:  filename - file name of the string
 * Return Value: 0 on success, -1 on failure
 * Function: Open the RTC devices. 
 */
int32_t open_rtc (const uint8_t* filename) {
    if (filename == NULL)
        return -1;                  // if the filename does not exist, return -1
    rtc_freq(2);
    return 0;
}

/* read_rtc
 * Inputs:  fd     - file descriptor
 *          buf    - data pointer
 *          nbytes - length of bytes taken
 * Return Value: 0 on success, -1 on failure
 * Function: Sleep until receiving an interrupt and read the interrupt rate  
 */
int32_t read_rtc (int32_t fd, void* buf, int32_t nbytes) {
    while (!rtc_int); // set a flag until the interrupt is handled (rtc_int = 0)
    rtc_int = 0;
    return 0;
}

/* write_rtc
 * Inputs:  fd     - file descriptor
 *          buf    - data pointer
 *          nbytes - length of bytes taken
 * Return Value: 0 on success, -1 on failure
 * Function: Write the interrupt rate  
 */
int32_t write_rtc (int32_t fd, const void* buf, int32_t nbytes) {
    // the system call will only accept a 4-byte integer for specifying the interrupt rate
    if (buf == NULL || nbytes != 4)
        return -1;
 	int32_t freq_int = *((int32_t *)buf);

    // check if the frequency is out of bounds and check if it is powers of two 
    if (freq_int < MIN_FREQ || freq_int > MAX_FREQ || (freq_int & (freq_int - 1)) != 0) 
        return -1;

    //printf("%u\n", freq_int);
    cli();
    rtc_freq(freq_int); 
    sti(); 
    return 0;
}

/* close_rtc
 * Input:  fd - file descriptor
 * Return Value: 0 on success, -1 on failure
 * Function: Close the RTC devices. 
 */
int32_t close_rtc (int32_t fd) {
    
    return 0;
}

/* rtc_freq
 * Inputs: freq -- frequency that is to be changed
 * Return Value: none
 * Function: Output rate from input frequency and change the freq of rtc 
 */
extern void rtc_freq (int32_t freq) {
    // int range;
    char rate; // frequency =  32768 >> (rate - 1);
    // periodic interrupt rate and Square-Wave Output Frequency 
    if (freq == 1024) 
    	rate = 0x06;	    //0110
    if (freq == 512) 
    	rate = 0x07;	    //0111
    if (freq == 256) 
    	rate = 0x08;	    //1000
    if (freq == 128) 
    	rate = 0x09;	    //1001
    if (freq == 64) 
    	rate = 0x0A;	    //1010
    if (freq == 32) 
    	rate = 0x0B;	    //1011
    if (freq == 16) 
    	rate = 0x0C;	    //1100
    if (freq == 8) 
    	rate = 0x0D;		//1101
    if (freq == 4) 
    	rate = 0x0E;		//1110
    if (freq == 2) 
    	rate = 0x0F;		//1111
    cli();
    outb(RTC_REG_A, RTC_PORT_1);                    // set index to register A
    char prev = inb(RTC_PORT_2);                    // get initial value of register A
/*
    if (rate >= MIN_RATE || rate <= MAX_RATE)       // rate will only be in the range from 3-15
        range = 1;
        return;
*/
    outb(RTC_REG_A, RTC_PORT_1);                           // reset index to A
    outb((prev & TOP_FOUR_BITMASK) | rate, RTC_PORT_2);    // write rate (the bottom 4 bits that represent the 
                                                           // periodic interrupt rate) to A.
    sti();
}



