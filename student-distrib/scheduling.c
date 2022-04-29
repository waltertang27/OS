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
int32_t curr_id; 

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
    curr_id = 0; 
   // page_table[VIDEO_PAGE_INDEX].page_table_addr = VIDEO_PAGE_INDEX + 2;
    enable_irq(PIT_IRQ_NUM);
}

extern void pit_handler(void) { 
    send_eoi(0); 
    scheduler();
}

extern void scheduler() {

    nextScheduled = ((currScheduled + 1) % 3);


    int32_t addr;
    currScheduledPID = terminals[currScheduled].currPID ;
    nextScheduledPID = terminals[nextScheduled].currPID ;  
 
     //Until first shell has booted
    if(currScheduledPID == -1 && nextScheduledPID == -1){
        send_eoi(0);
        return; 
    }

    pcb_t *pcb = get_pcb(currScheduledPID);

    // save esp, ebp to current pcb
    asm volatile(
        "movl %%esp, %%edx \n "
        "movl %%ebp, %%ecx \n "
        : "=d"(pcb->save_esp), "=c"(pcb->save_ebp)
        : 
        : "memory"
    );

    page_directory[VIDMAP_INDEX].page_table_addr = (int32_t)video_mapping_pt / ALIGN_BYTES; 
    video_mapping_pt[0].page_table_addr = VID_ADDR / ALIGN_BYTES;

    if (nextScheduledPID == -1)
    {
        send_eoi(0);
         // int prev = terminal_flag; 
         // terminal_flag = nextScheduled; 
        // switch_terminals(prev); 
         currScheduled = nextScheduled; 
        

        //Do some sort of mapping so this shell prints in another terminal
        execute("shell");
        return; 
    }
    else{
        page_table[VIDEO_PAGE_INDEX].page_table_addr = VIDEO_PAGE_INDEX;

        pcb = get_pcb(terminals[nextScheduled].currPID);

        addr = EIGHTMB + ((nextScheduledPID)*PAGE_SIZE);
        page_directory[USER_INDEX].page_table_addr = addr / ALIGN_BYTES;
        currScheduled = nextScheduled;
    }

    flush_tlb();
    
    tss.esp0 = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * pcb->process_id);
    tss.ss0 = KERNEL_DS;


    send_eoi(PIT_IRQ_NUM);

    int32_t ebpSave = pcb->save_ebp;
    int32_t espSave = pcb->save_esp;

    asm volatile(
        "movl %%edx, %%esp \n "
        "movl %%ecx, %%ebp \n "
        :
        : "d"(espSave), "c"(ebpSave)
        : "memory");

}


