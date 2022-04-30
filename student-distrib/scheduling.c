#include "scheduling.h"
#include "lib.h"
#include "i8259.h"
#include "systemcalls.h"
#include "paging.h"
#include "types.h"
#include "terminal.h"
#include "x86_desc.h"

// volatile uint8_t prev = 0;
// volatile uint8_t cur = 0;
extern int terminal_flag; 
extern void flush_tlb();
extern int32_t curr_id ;
int i; 
// Curent terminal that is being executed
int currScheduled; 
int nextScheduled; 
int currScheduledPID; 
int nextScheduledPID; 



extern void pit_init(void) {
    outb(FREQ_SET, MC_REG);
    outb(PIT_FREQ & PIT_MASK, CHAN_0);
    outb(PIT_FREQ >> SHIFT, CHAN_0);
    currScheduled = 0; 

  //  terminals[0].shellRunning = 1; 
    enable_irq(PIT_IRQ_NUM);
}

extern void pit_handler(void) { 
    send_eoi(0);

    if(terminals[1].shellRunning == 1 || terminals[2].shellRunning == 1)
        scheduler();
}

extern void scheduler() {
    int32_t addr;
    nextScheduled = (currScheduled + 1) % 3; 
    currScheduledPID = terminals[currScheduled].currPID ;
    nextScheduledPID = terminals[nextScheduled].currPID ;  


    if(currScheduledPID == -1 || nextScheduledPID == -1 || terminals[nextScheduled].shellRunning == -1 || terminals[currScheduled].shellRunning == -1){
        send_eoi(0);
        currScheduled = nextScheduled; 
        return; 
    }

    // video_mapping_pt[0].page_table_addr = (VIDEO_PAGE_INDEX + nextScheduled + 1); 

     video_mapping_pt[0].page_table_addr = VID_ADDR / ALIGN_BYTES;
    page_table[VIDEO_PAGE_INDEX].page_table_addr = VID_ADDR / ALIGN_BYTES;


    // uint8_t * vidmapStart; 
    // vidmap(&vidmapStart); 


    // get current process; terminal_flag tells us the current terminal
    pcb_t *pcb = get_pcb(currScheduledPID);
    pcb_t* newPcb = get_pcb(nextScheduledPID); 

    if (terminal_flag != nextScheduled){
        video_mapping_pt[0].page_table_addr = (VID_ADDR + (nextScheduled + 1) * FOURKB) / ALIGN_BYTES;
        page_table[VIDEO_PAGE_INDEX].page_table_addr = (VIDEO_PAGE_INDEX + nextScheduled + 1) ;
    }

    flush_tlb();

    currScheduled = nextScheduled; 

    tss.esp0 = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * pcb->process_id);
    tss.ss0 = KERNEL_DS;

    // save esp, ebp to current pcb
    asm volatile(
        "movl %%esp, %%edx \n "
        "movl %%ebp, %%ecx \n "
        : "=d"(pcb->task_esp), "=c"(pcb->task_ebp)
        : 
        : "memory"
    );



       asm volatile(
        "movl %%ecx, %%ebp \n "
        "movl %%edx, %%esp \n "
        :
        : "c"(newPcb->task_ebp), "d"(newPcb->task_esp)
        : "memory"
    );

    flush_tlb();
}



