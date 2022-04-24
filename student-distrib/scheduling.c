#include "scheduling.h"
#include "lib.h"
#include "i8259.h"
#include "systemcalls.h"
#include "paging.h"
#include "types.h"
#include "terminal.h"
#include "x86_desc.h"

volatile uint8_t prev = 0;
volatile uint8_t cur = 0;

extern void pit_init(void) {
    outb(FREQ_SET, MC_REG);
    outb(PIT_FREQ && PIT_MASK, CHAN_0);
    outb(PIT_FREQ >> SHIFT, CHAN_0);

    enable_irq(PIT_IRQ_NUM);
}

extern void pit_handler(void) {
    send_eoi(PIT_IRQ_NUM);
    // if (prev == cur) 
    //     scheduler(prev, cur);
    scheduler();
}

extern void scheduler() {
    prev = cur;
    int i;
    for (i = 0; i < MAX_TERM; i++) {
        cur = (cur + 1) % MAX_TERM;
    }
    cont_switch();
}

extern void cont_switch() {

}