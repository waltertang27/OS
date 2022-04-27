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
    int curid = terminals[terminal_flag].currPID;
    int next_id;


    // if no process is runnning at current terminal
    if(pcb == NULL)
    {
        send_eoi(PIT_IRQ_NUM);
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

    next_id = curid;
    next_id = (next_id + 1);
    next_id = next_id % MAX_TERM;

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

    // if running process is not on visible terminal
    if (terminal_flag != curid){
        video_mapping_pt[0].page_table_addr = (VID_ADDR + (curid + 1) * FOURKB) / ALIGN_BYTES;
        page_table[VIDEO_PAGE_INDEX].page_table_addr = (VID_ADDR + (curid + 1) * FOURKB) / ALIGN_BYTES;
    }

    flush_tlb();
    
    pcb = (pcb_t *)terminals[terminal_flag].currPCB;

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

