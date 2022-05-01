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
int sum; 


extern int32_t curr_id ;
// Curent terminal that is being executed
int currScheduled; 
int nextScheduled; 
int currScheduledPID; 
int nextScheduledPID; 
int typingFlag; 


extern void save_reg(void * savePCB){

        asm volatile(
        "movl %%ebp, %0 \n "
        "movl %%esp, %1 \n "
        : "=r"(((pcb_t *)savePCB)->task_ebp), "=r"(((pcb_t *)savePCB)->task_esp)
        : 
        : "memory"
    );
}


extern void pit_init(void) {
    outb(FREQ_SET, MC_REG);
    outb(PIT_FREQ & PIT_MASK, CHAN_0);
    outb(PIT_FREQ >> SHIFT, CHAN_0);
    currScheduled = 2; 
    finalFlag = 0; 
    sum = -1; 
    typingFlag = 0; 
  //  terminals[0].shellRunning = 1; 
    enable_irq(PIT_IRQ_NUM);
}

extern void pit_handler(void) { 
    send_eoi(0);

    int i; 
    sum++; 

    if(sum > 100)
        sum = 5; 


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

         asm volatile(
        "movl %%ebp, %0 \n "
        "movl %%esp, %1 \n "
        : "=r"((get_pcb(0))->task_ebp), "=r"((get_pcb(0))->task_esp)
        : 
        : "memory"
    );

        execute("shell"); //get_pcb(1)
        return; 
        break; 
    case 2: 
        terminal_flag = 2; 
        switch_terminals(1); 
        terminals[2].shellRunning = 1; 

        asm volatile(
        "movl %%ebp, %0 \n "
        "movl %%esp, %1 \n "
        : "=r"((get_pcb(1))->task_ebp), "=r"((get_pcb(1))->task_esp)
        : 
        : "memory"
        );

        execute("shell"); 
        return; 
        break;
    case 3: 
          asm volatile(
        "movl %%ebp, %0 \n "
        "movl %%esp, %1 \n "
        : "=r"((get_pcb(2))->task_ebp), "=r"((get_pcb(2))->task_esp)
        : 
        : "memory"
        );
        return; 
        break; 
        //By this point esp and ebp of terminal 1 2 and 3 will be set and we can begin to call scheduler
    default:
        break;
    }
    if(!typingFlag){
        nextScheduled = (currScheduled + 1) % 3; 
        scheduler(nextScheduled);
    }
}

extern void scheduler(int next) {
    int32_t addr;
    currScheduledPID = terminals[currScheduled].currPID ;
    nextScheduledPID = terminals[next].currPID ;  
    //Uncomment line to see if paging is working right 
    
    // printf("Running %d switching %d\n",currScheduledPID,nextScheduledPID); 

    if(currScheduledPID == -1 && nextScheduledPID == -1){
        send_eoi(0);
        currScheduled = next; 
        return; 
    }
        
    // get current process; terminal_flag tells us the current terminal
    pcb_t *pcb = get_pcb(currScheduledPID);


   asm volatile(
        "movl %%ebp, %0 \n "
        "movl %%esp, %1 \n "
        : "=r"(((pcb_t *)pcb)->task_ebp), "=r"(((pcb_t *)pcb)->task_esp)
        : 
        : "memory"
    );


   // video_mapping_pt[0].page_table_addr = VID_ADDR / ALIGN_BYTES;


    currScheduled = next; 

    pcb = get_pcb(nextScheduledPID);

   if(pcb->task_ebp == 0 || pcb->task_esp == 0){
       return; 
    }

  //  if running process is not on visible terminal
    if (terminal_flag != next){
        // video_mapping_pt[0].page_table_addr = (VID_ADDR + (nextScheduled + 1) * FOURKB) / ALIGN_BYTES;
        page_table[VIDEO_PAGE_INDEX].page_table_addr = (VIDEO_PAGE_INDEX + next + 1) ;
    }
    else{
        page_table[VIDEO_PAGE_INDEX].page_table_addr = VIDEO_PAGE_INDEX; 
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


