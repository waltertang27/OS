#include "paging.h"

// extern void enable(int directory);



/*
	void init()
	Description: initialize paging
	Inputs: None
	Outputs: None
*/

void paging_init(){

	

	int i;

	for (i = 0; i < TABLE_SIZE; i++){
		if (i == 0){
			// first entry
			page_directory[i].present = 1;
			page_directory[i].user_supervisor = 0;
			page_directory[i].page_table_addr = ((int)page_table) / ALIGN_BYTES;
		} else if (i == KERNEL_INDEX){
			// kernel memory
			page_directory[i].present = 1;
			page_directory[i].user_supervisor = 0;
			page_directory[i].page_table_addr = KERNEL_ADDR / ALIGN_BYTES;
		} else if (i == USER_INDEX){
			// user memory
			page_directory[i].present = 1;
			page_directory[i].user_supervisor = 1;
			page_directory[i].page_table_addr = USER_V / ALIGN_BYTES;
		} else {
			// not setting up page_table_addr
			page_directory[i].present = 0;
			page_directory[i].user_supervisor = 0;
		}

		page_directory[i].read_write = 1;
		page_directory[i].write_through = 0;
		page_directory[i].cache_disable = 0;
		page_directory[i].accessed = 0;
		page_directory[i].reserved = 0;
		
		if (i == 0){
			page_directory[i].page_size = 0;
		} else {
			page_directory[i].page_size = 1;
		}
  	}
	
  	// page table
	for (i = 0; i < TABLE_SIZE; i++){
		// video memory
		if (i * ALIGN_BYTES == VID_ADDR){
			page_table[i].present   = 1;
		} else {
			page_table[i].present   = 0;
		}
		page_table[i].read_write = 1;
		page_table[i].user_supervisor = 0;
		page_table[i].write_through = 0;
		page_table[i].cache_disable = 0;
		page_table[i].accessed = 0;
		page_table[i].dirty = 0;
		page_table[i].reserved = 0;
		page_table[i].global = 0;
		page_table[i].page_table_addr = i;
	}
	
	// video mapping
	for (i = 0; i < TABLE_SIZE; i++){
		video_mapping_pt[i].present = 0;
		video_mapping_pt[i].read_write = 1;
		video_mapping_pt[i].user_supervisor = 0;
		video_mapping_pt[i].write_through = 0;
		video_mapping_pt[i].cache_disable = 1;
		video_mapping_pt[i].accessed = 0;
		video_mapping_pt[i].dirty = 0;
		video_mapping_pt[i].reserved = 0;
		video_mapping_pt[i].global = 0;
		video_mapping_pt[i].page_table_addr = i;
  	}
	// enable((int)page_directory);
	
	/*
	asm (
		"movl $page_directory, %%eax ;"
		"andl $0xFFFFFC00, %%eax ;"
		"movl %%eax, %%cr3 ;"
		"movl %%cr4, %%eax ;"
		"orl $0x00000010, %%eax ;"
		"movl %%eax, %%cr4 ;"
		"movl %%cr0, %%eax ;"
		"orl $0x80000000, %%eax ;"
		"movl %%eax, %%cr0"
		: : : "eax", "cc" );
	*/

}
