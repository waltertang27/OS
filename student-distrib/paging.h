#ifndef PAGING_H
#define PAGING_H
#ifndef ASM

#include "types.h"
#include "x86_desc.h"
#include "lib.h"

#define TABLE_SIZE 1024
#define ALIGN_BYTES 4096  // 4KB, 1024 * 4
#define PAGE_SIZE 4194304 // 4Mb

#define KERNEL_ADDR 0x400000
#define VID_ADDR 0xB8000
#define USER_V 0x8000000 // user virtual

#define KERNEL_INDEX 1
#define USER_INDEX 32

typedef struct __attribute__(()) page_directory_entry
{
	uint8_t present : 1;
	uint8_t read_write : 1;
	uint8_t user_supervisor : 1;
	uint8_t write_through : 1;
	uint8_t cache_disable : 1;
	uint8_t accessed : 1;
	uint8_t reserved : 1;
	uint8_t page_size : 1;
	uint8_t avl : 3;
	uint32_t page_table_addr : 20;
} page_directory_entry_t;

typedef struct __attribute__(()) page_table_entry
{
	uint8_t present : 1;
	uint8_t read_write : 1;
	uint8_t user_supervisor : 1;
	uint8_t write_through : 1;
	uint8_t cache_disable : 1;
	uint8_t accessed : 1;
	uint8_t reserved : 1;
	uint8_t dirty : 1;
	uint8_t global : 1;
	uint8_t avl : 3;
	uint32_t page_table_addr : 20;
} page_table_entry_t;

page_directory_entry_t page_directory[TABLE_SIZE] __attribute__((aligned(ALIGN_BYTES)));
page_table_entry_t page_table[TABLE_SIZE] __attribute__((aligned(ALIGN_BYTES)));
page_table_entry_t video_mapping_pt[TABLE_SIZE] __attribute__((aligned(ALIGN_BYTES)));

extern void paging_init();

#endif
#endif
