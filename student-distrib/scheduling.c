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
int finalFlag; 


extern int32_t curr_id ;
// Curent terminal that is being executed
int currScheduled; 
int nextScheduled; 
int currScheduledPID; 
int nextScheduledPID; 


extern void save_reg(void * savePCB){

        asm volatile(
        "movl %%ebp, %%ecx \n "
        "movl %%esp, %%edx \n "
        : "=c"(((pcb_t *)savePCB)->task_ebp), "=d"(((pcb_t *)savePCB)->task_esp)
        : 
        : "memory"
    );
}


extern void pit_init(void) {
    outb(FREQ_SET, MC_REG);
    outb(PIT_FREQ & PIT_MASK, CHAN_0);
    outb(PIT_FREQ >> SHIFT, CHAN_0);
    currScheduled = 2; 

  //  terminals[0].shellRunning = 1; 
    enable_irq(PIT_IRQ_NUM);
}

extern void pit_handler(void) { 
    send_eoi(0);

    int i,sum;
    sum = 0;  
    for(i = 0;i<PROCESS_ARRAY_SIZE;i++)
        sum += process_array[i];

    switch (sum)
    {
    case 0:
        finalFlag = 0; 
        terminal_flag = 0;
        terminals[0].shellRunning = 1; 
        execute("shell"); 
        return;  
        break;
    case 1: 
        terminal_flag = 1; 
        switch_terminals(0); 
        terminals[1].shellRunning = 1; 
        save_reg((void *)get_pcb(0)); 
        execute("shell"); 
        return; 
        break; 
    case 2: 
        terminal_flag = 2; 
        switch_terminals(1); 
        terminals[2].shellRunning = 1; 
        save_reg((void *)get_pcb(1)); 
        execute("shell"); 
        return; 
        break;
    case 3: 
        save_reg((void *)get_pcb(2)); 
        finalFlag = 1; 
        break; 
    case 4: 
        finalFlag = 0; 
        break; 
    default:
        break;
    }
 
  //  printf("sum: %d\n",sum);
     scheduler();
}

extern void scheduler() {
    int32_t addr;

    nextScheduled = (currScheduled + 1) % 3; 
    currScheduledPID = terminals[currScheduled].currPID ;
    nextScheduledPID = terminals[nextScheduled].currPID ;  


    if(currScheduledPID == -1 && nextScheduledPID == -1){
        send_eoi(0);
        currScheduled = nextScheduled; 
        return; 
    }
        

    // get current process; terminal_flag tells us the current terminal
    pcb_t *pcb = get_pcb(currScheduledPID);

    // if no process is runnning at current terminal

    // save esp, ebp to current pcb
    if(!finalFlag)
        save_reg((void *)pcb); 

    video_mapping_pt[0].page_table_addr = VID_ADDR / ALIGN_BYTES;
    page_table[VIDEO_PAGE_INDEX].page_table_addr = VIDEO_PAGE_INDEX; 

    currScheduled = nextScheduled; 

    pcb = get_pcb(nextScheduledPID);

  //  if running process is not on visible terminal
    if (terminal_flag != nextScheduled){
        // video_mapping_pt[0].page_table_addr = (VID_ADDR + (nextScheduled + 1) * FOURKB) / ALIGN_BYTES;
        page_table[VIDEO_PAGE_INDEX].page_table_addr = (VIDEO_PAGE_INDEX + nextScheduled + 1) ;
    }

    addr = EIGHTMB + ((nextScheduledPID)*PAGE_SIZE);
    page_directory[USER_INDEX].page_table_addr = addr / ALIGN_BYTES;

    flush_tlb();

    tss.esp0 = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * pcb->process_id);
    tss.ss0 = KERNEL_DS;


     asm volatile(
        "movl %0, %%ebp \n "
        "movl %1, %%esp \n "
        :
        : "r"(pcb->task_ebp), "r"(pcb->task_esp)
        : "memory"
    );

}


