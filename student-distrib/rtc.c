
#include "rtc.h" 

extern void rtc_init(void){
    printf("RTC Started init \n");

    outb(0x8B, 0x70);
    char prev =inb(0x71);
    outb(0x8B,0x70);
    outb(prev | 0x40,0x71);


    outb(0x8A,0x70);
    prev =inb(0x71);
    outb(0x8A,0x70);
    outb(prev & 0xF0 | 15,0x71);

    enable_irq(8);
}

extern void rtc_handler(void){
    cli();
   // printf("Called handler \n");
   // test_interrupts();
    outb(0x8C, 0x70);	// select register C
    inb(0x71);		// just throw away contents
    send_eoi(8);
    sti();
    // plz work
}
