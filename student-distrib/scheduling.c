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
extern int terminal_flag; 

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
    // get current process; terminal_flag tells us the current terminal
    pcb_t new_process;
    pcb_t * pcb = (pcb_t *)terminals[terminal_flag].currPCB;

    // if no process is runnning at current terminal
    if(pcb == NULL)
    {
        // create new process
        asm volatile(
            "movl %%esp, %%edx \n "
            "movl %%ebp, %%ecx \n "
            : "=d"(new_process.save_esp), "=c"(new_process.save_ebp)
            : 
            : "memory"
        );
        
        terminals[terminal_flag].currPCB = (void *)&new_process;

        // switch terminal
        switch_terminals(terminal_flag);

        // send eoi and execute shell
        send_eoi(PIT_IRQ_NUM);
        execute((uint8_t * )"shell");
    }

    // save esp, ebp to current pcb
    asm volatile(
        "movl %%esp, %%edx \n "
        "movl %%ebp, %%ecx \n "
        : "=d"(pcb->save_esp), "=c"(pcb->save_ebp)
        : 
        : "memory"
    );

    // round robin
    prev = cur;
    cur = (cur + 1);
    cur = cur % MAX_TERM;


    // more asm

    pcb = (pcb_t *)terminals[terminal_flag].currPCB;

    cont_switch();
}

extern void cont_switch() {
    // context switch, save esp and ebp
    pcb_t * pcb = terminals[terminal_flag].currPCB;
    int32_t ebpSave = pcb->save_ebp; 
    int32_t espSave = pcb->save_esp;

    asm volatile(
        "movl %%edx, %%esp \n "
        "movl %%ecx, %%ebp \n "
        :
        : "d"(espSave), "c"(ebpSave)
        : "memory"
    );

}

