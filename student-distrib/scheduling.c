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
    
    // send_eoi(PIT_IRQ_NUM);
    // if (prev == cur) 
    //     scheduler(prev, cur);
    scheduler();
}

extern void scheduler() {
    currScheduled = currScheduled % 3; 

    if(terminals[currScheduled].shellRunning != 1){
        send_eoi(0);
        return; 
    }
        

    // get current process; terminal_flag tells us the current terminal
    pcb_t * pcb = get_pcb( terminals[currScheduled].currPID);

    if(pcb == NULL){
        send_eoi(0);
        return; 
    }


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
    page_directory[VIDMAP_INDEX].user_supervisor = 1;
    page_directory[VIDMAP_INDEX].present = 1;
    page_directory[VIDMAP_INDEX].page_size = 0;
    page_directory[VIDMAP_INDEX].read_write = 1;
    page_directory[VIDMAP_INDEX].write_through = 0;
    page_directory[VIDMAP_INDEX].cache_disable = 0;
    page_directory[VIDMAP_INDEX].accessed = 0;
    page_directory[VIDMAP_INDEX].dirty = 0;
    page_directory[VIDMAP_INDEX].page_table_addr = (int32_t)video_mapping_pt / ALIGN_BYTES; // points to the video mapping page table

    // setup video mapping table entry
    video_mapping_pt[0].user_supervisor = 1;
    video_mapping_pt[0].present = 1;
    video_mapping_pt[0].read_write = 1;
    video_mapping_pt[0].write_through = 0;
    video_mapping_pt[0].cache_disable = 0;
    video_mapping_pt[0].accessed = 0;
    video_mapping_pt[0].dirty = 0;
    video_mapping_pt[0].attribute = 0;
    video_mapping_pt[0].global = 0;
    video_mapping_pt[0].page_table_addr = VID_ADDR / ALIGN_BYTES;



    // video memory into video page
    page_table[VIDEO_PAGE_INDEX].user_supervisor = 1;
    page_table[VIDEO_PAGE_INDEX].present = 1;
    page_table[VIDEO_PAGE_INDEX].read_write = 1;
    page_table[VIDEO_PAGE_INDEX].write_through = 0;
    page_table[VIDEO_PAGE_INDEX].cache_disable = 0;
    page_table[VIDEO_PAGE_INDEX].accessed = 0;
    page_table[VIDEO_PAGE_INDEX].dirty = 0;
    page_table[VIDEO_PAGE_INDEX].attribute = 0;
    page_table[VIDEO_PAGE_INDEX].global = 0;
    page_table[VIDEO_PAGE_INDEX].page_table_addr = VID_ADDR / ALIGN_BYTES;


    currScheduled++; 
    currScheduled %=3 ; 

    if(terminals[currScheduled].shellRunning != 1){
        send_eoi(0);
        return; 
    }
    pcb = get_pcb( terminals[currScheduled].currPID);

    if(pcb == NULL){
        send_eoi(0);
        return; 
    }




    // if running process is not on visible terminal
    if (terminal_flag != currScheduled){
        video_mapping_pt[0].page_table_addr = (VID_ADDR + (currScheduled + 1) * FOURKB) / ALIGN_BYTES;
        page_table[VIDEO_PAGE_INDEX].page_table_addr = (VID_ADDR + (currScheduled + 1) * FOURKB) / ALIGN_BYTES;
    }

    // paging
    page_directory[USER_INDEX].user_supervisor = 1;
    page_directory[USER_INDEX].present = 1;
    page_directory[USER_INDEX].page_size = 1;
    page_directory[USER_INDEX].read_write = 1;
    page_directory[USER_INDEX].write_through = 0;
	page_directory[USER_INDEX].cache_disable = 0;
	page_directory[USER_INDEX].accessed = 0;
	page_directory[USER_INDEX].dirty = 0;
    page_directory[USER_INDEX].page_table_addr = (int32_t)(EIGHTMB + pcb->process_id * PAGE_SIZE) / ALIGN_BYTES;


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

}


