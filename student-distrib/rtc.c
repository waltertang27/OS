
#include "rtc.h" 

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
    
    rtc_rate = MIN_FREQ; // the initial frequency is set to 2 interrupts/second;
    rtc_int = 1; // interrupts occur and not handled yet
    rtc_freq(MAX_FREQ);
    
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
    if(testing_RTC)
        printf("Called handler \n");
    //test_interrupts();
    outb(RTC_REG_C, RTC_PORT_1);	// select register C
    inb(RTC_PORT_2);		        // just throw away contents
    send_eoi(RTC_IRQ_NUM);
    sti();
}

int32_t open_rtc (const uint8_t* filename) {
    if (filename == NULL)
        return -1;                  // if the filename does not exist, return -1
    rtc_rate = MIN_FREQ;
    return 0;
}

int32_t read_rtc (int32_t fd, void* buf, int32_t nbytes) {
    sti();
    while (rtc_int != 1); // set a flag until the interrupt is handled (rtc_int = 0)
    cli();
    rtc_int = 0;
    return 0;
}

int32_t write_rtc (int32_t fd, const void* buf, int32_t nbytes) {
    // the system call will only accept a 4-byte integer for specifying the interrupt rate
    if (buf == NULL || nbytes != sizeof(int32_t))
        return -1;
 	int32_t freq_int = *(int32_t *)buf;
    // check if the frequency is out of bounds and check if it is powers of two 
    if (freq_int < MIN_FREQ || freq_int > MAX_FREQ || (freq_int & (freq_int - 1)) != 0) 
        return -1;

    rtc_rate = MAX_FREQ / freq_int; 
    return 0;
}

int32_t close_rtc (int32_t fd) {
    return 0;
}

extern void rtc_freq (int32_t freq) {
    // int range;
    char rate = rate_(32768) - (rate_(freq) - 1);   // frequency =  32768 >> (rate - 1);
    outb(RTC_REG_A, RTC_PORT_1);                    // set index to register A
    unsigned char prev = inb(RTC_PORT_2);           // get initial value of register A
    freq &= 0x0F;                                   // get the lower 4 bits

/*
    if (rate >= MIN_RATE || rate <= MAX_RATE)       // rate will only be in the range from 3-15
        range = 1;
        return;
*/
    outb(RTC_REG_A, RTC_PORT_1);                           // reset index to A
    outb((prev & TOP_FOUR_BITMASK) | rate, RTC_PORT_2);    // write rate (the bottom 4 bits that represent the 
                                                           // periodic interrupt rate) to A.
}

char rate_(uint32_t freq) {                                 // log2 operation
    char log2 = 0;
    while (freq % 2 != 1) {
        freq >>= 1;
        log2++;
    }
    return log2;
}


