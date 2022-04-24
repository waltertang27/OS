#include "scheduling.h"
#include "lib.h"
#include "i8259.h"
#include "systemcalls.h"
#include "paging.h"
#include "types.h"
#include "terminal.h"
#include "x86_desc.h"

volatile uint8_t cur = 0;
volatile uint8_t prev = 0;

extern void pit_init(void) {
    outb(FREQ_SET, MC_REG);
    outb(PIT_FREQ & LOW_MASK, CHAN_0);
    outb(PIT_FREQ >> SHIFT, CHAN_0);

    enable_irq(PIT_IRQ_NUM);
}

extern void pit_handler(void) {
    send_eoi(PIT_IRQ_NUM);
    prev = cur;
    
}