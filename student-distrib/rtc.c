
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
    outb(prev & TOP_FOUR_BITMASK | LOWER_FOUR_BITMASK ,RTC_PORT_2); //0F allows to mask for top 4 bits
    testing_RTC = 0; 
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
