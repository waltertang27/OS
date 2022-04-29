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

    int i,sum;
    sum = 0;  
    for(i = 0;i<PROCESS_ARRAY_SIZE;i++)
        sum += process_array[i];

    switch (sum)
    {
    case 0:
        terminal_flag = 0;
         // terminals[0].shellRunning = 1; 
        execute("shell");  
        break;
    case 1: 
        terminal_flag = 1; 
        switch_terminals(0); 
        terminals[1].shellRunning = 1; 
        execute("shell"); 
    case 2: 
        terminal_flag = 2; 
        switch_terminals(1); 
        terminals[2].shellRunning = 1; 
        execute("shell"); 
    default:
       // curr_id = terminals[terminal_flag].currPID ; 
        break;
    }
 
  //  printf("sum: %d\n",sum);
    scheduler();
}

extern void scheduler() {
    nextScheduled = (currScheduled + 1) % 3; 
    int32_t addr;
    currScheduledPID = terminals[currScheduled].currPID ;
    nextScheduledPID = terminals[nextScheduled].currPID ;  


    if(currScheduledPID == -1 && nextScheduledPID == -1){
        send_eoi(0);
        return; 
    }
        

    // get current process; terminal_flag tells us the current terminal
    pcb_t *pcb = get_pcb(currScheduledPID);


    // if no process is runnning at current terminal

    // save esp, ebp to current pcb
    asm volatile(
        "movl %%esp, %%edx \n "
        "movl %%ebp, %%ecx \n "
        : "=d"(pcb->save_esp), "=c"(pcb->save_ebp)
        : 
        : "memory"
    );

    // round robin
    // next_id = terminals[currScheduled].currPID ; 

    /* setup video memory (from vidmap) */
    // setup page directory entry

    // setup video mapping table entry

    video_mapping_pt[0].page_table_addr = VID_ADDR / ALIGN_BYTES;
    page_table[VIDEO_PAGE_INDEX].page_table_addr = VIDEO_PAGE_INDEX; 

    currScheduled = nextScheduled; 

    pcb = get_pcb(nextScheduledPID);

    if(pcb == NULL){
        send_eoi(0);
        return; 
    }

  //  if running process is not on visible terminal
    if (terminal_flag != nextScheduled){
        video_mapping_pt[0].page_table_addr = (VID_ADDR + (nextScheduled + 1) * FOURKB) / ALIGN_BYTES;
        page_table[VIDEO_PAGE_INDEX].page_table_addr = (VIDEO_PAGE_INDEX + nextScheduled + 1) ;
    }

    addr = EIGHTMB + ((nextScheduledPID)*PAGE_SIZE);
    page_directory[USER_INDEX].page_table_addr = addr / ALIGN_BYTES;

    flush_tlb();
    tss.esp0 = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * pcb->process_id);
    tss.ss0 = KERNEL_DS;

    send_eoi(PIT_IRQ_NUM);

    int32_t ebpSave = pcb->save_ebp; 
    int32_t espSave = pcb->save_esp;

     asm volatile(
        "movl %%ecx, %%ebp \n "
        "movl %%edx, %%esp \n "
        :
        : "c"(ebpSave), "d"(espSave)
        : "memory"
    );
}

extern void cont_switch() {
    // context switch, save esp and ebp
    pcb_t * pcb = terminals[currScheduled].currPCB;
    int32_t ebpSave = pcb->save_ebp; 
    int32_t espSave = pcb->save_esp;
    

    asm volatile(
        "movl %%ecx, %%ebp \n "
        :
        : "c"(ebpSave)
        : "memory"
    );
    asm volatile (
        "andl $0x00FF, %%edx \n "
        "movw %%dx,%%ds \n "
        "pushl %%edx \n"
        "pushl %%ecx \n" 
        "pushfl      \n"
        "popl %%ecx  \n"
        "orl $0x0200, %%ecx   \n"
        "pushl %%ecx \n"
        "pushl %%ebx \n"
        "movl label1, %%eax \n"
        "pushl %%eax \n"
        "iret \n"
        "label1: \n"
        :
        : "b"(USER_CS), "c"(espSave), "d"(USER_DS)
        : "memory" 
    );
}


