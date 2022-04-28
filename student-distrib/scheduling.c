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
// Curent terminal that is being executed
int currScheduled; 

extern void pit_init(void) {
    outb(FREQ_SET, MC_REG);
    outb(PIT_FREQ & PIT_MASK, CHAN_0);
    outb(PIT_FREQ >> SHIFT, CHAN_0);
    currScheduled = 0; 
    enable_irq(PIT_IRQ_NUM);
}

extern void pit_handler(void) {
    send_eoi(PIT_IRQ_NUM);
    // if (prev == cur) 
    //     scheduler(prev, cur);
    scheduler();
}

extern void scheduler() {
    currScheduled = currScheduled % 3; 


    if(terminals[currScheduled+1].shellRunning != 1)
        return; 

    // get current process; terminal_flag tells us the current terminal
    // pcb_t new_process;
    // int next_id; 
    pcb_t * pcb = get_pcb(currScheduled);

    // if no process is runnning at current terminal
    if((void *)pcb == NULL )
    {
        return; 
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
    // next_id = terminals[currScheduled].currPID ; 

    /* setup video memory (from vidmap) */
    // setup page directory entry
    setup_mem(VIDMAP_INDEX,(int32_t)video_mapping_pt / ALIGN_BYTES,2); 

    // setup video mapping table entry
    setup_mem(0,VID_ADDR/ALIGN_BYTES,1); 


    // video memory into video page
    setup_mem(VIDEO_PAGE_INDEX,(VID_ADDR / ALIGN_BYTES),0); 


    // if running process is not on visible terminal
    if (terminal_flag != currScheduled){
        video_mapping_pt[0].page_table_addr = (VID_ADDR + (currScheduled + 1) * FOURKB) / ALIGN_BYTES;
        page_table[VIDEO_PAGE_INDEX].page_table_addr = (VID_ADDR + (currScheduled + 1) * FOURKB) / ALIGN_BYTES;
    }

    flush_tlb();
    
    pcb = (pcb_t *)terminals[currScheduled].currPCB;

    // paging
    setup_mem(USER_INDEX,((int32_t)(EIGHTMB + pcb->process_id * PAGE_SIZE) / ALIGN_BYTES),0);

    flush_tlb();


    tss.esp0 = EIGHTMB - SIZE_OF_INT32 - (EIGHTKB * pcb->process_id);
    tss.ss0 = KERNEL_DS;
    send_eoi(PIT_IRQ_NUM);
    cont_switch();
}

extern void cont_switch() {
    // context switch, save esp and ebp
    pcb_t * pcb = terminals[currScheduled].currPCB;
    int32_t ebpSave = pcb->save_ebp; 
    int32_t espSave = pcb->save_esp;

    asm volatile(
        "movl %%edx, %%esp \n "
        "movl %%ecx, %%ebp \n "
        :
        : "d"(espSave), "c"(ebpSave)
        : "memory"
    );
    
    currScheduled++; 
}


void setup_mem(int index, uint32_t page_address, int table){
        switch (table)
        {
        case 1:
            video_mapping_pt[index].user_supervisor = 1;
            video_mapping_pt[index].present = 1;
            video_mapping_pt[index].read_write = 1;
            video_mapping_pt[index].write_through = 0;
            video_mapping_pt[index].cache_disable = 0;
            video_mapping_pt[index].accessed = 0;
            video_mapping_pt[index].dirty = 0;
            video_mapping_pt[index].attribute = 0;
            video_mapping_pt[index].global = 0;
            video_mapping_pt[index].page_table_addr = page_address; 
            break;
        case 2: 
            page_directory[index].user_supervisor = 1;
            page_directory[index].present = 1;
            page_directory[index].page_size = 1;
            page_directory[index].read_write = 1;
            page_directory[index].write_through = 0;
            page_directory[index].cache_disable = 0;
            page_directory[index].accessed = 0;
            page_directory[index].dirty = 0;
            page_directory[index].page_table_addr = page_address ; 
        case 3:
            page_table[index].user_supervisor = 1;
            page_table[index].present = 1;
            page_table[index].read_write = 1;
            page_table[index].write_through = 0;
            page_table[index].cache_disable = 0;
            page_table[index].accessed = 0;
            page_table[index].dirty = 0;
            page_table[index].attribute = 0;
            page_table[index].global = 0;
            page_table[index].page_table_addr = VID_ADDR / ALIGN_BYTES;
            break;
        }
}


