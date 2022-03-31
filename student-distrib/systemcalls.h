#ifndef SYSTEMCALLS_H
#define SYSTEMCALLS_H

#include "types.h"
#include "x86_desc.h"
#include "lib.h"

#define STDIN 0
#define STDOUT 1
#define FD_START_INDEX 2 // file descriptor start index, not including stdin (0) or stdout (1).

#define MAGIC_1 0x7f
#define MAGIC_2 0x45
#define MAGIC_3 0x4c
#define MAGIC_4 0x46

int32_t halt (uint8_t status); //3.1
int32_t execute (const uint8_t* command); //3.1
int32_t read (int32_t fd, void* buf, int32_t nbytes); //3.1
int32_t write (int32_t fd, const void* buf, int32_t nbytes); //3.1
int32_t open (const uint8_t* filename); //3.1
int32_t close (int32_t fd); //3.1
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen start);
int32_t set_handler (int32_t signum, void* handler address);
int32_t sigreturn (void);


// per-task data structures, to support tasks

typedef struct fileop_jmp_table {
    // not sure data type yet
    int32_t open;
    int32_t read;
    int32_t write;
    int32_t close;
} fileop_jmp_table_t;

typedef struct file_descriptor {
    fileop_jmp_table_t jump_table;
    int32_t inode;
    int32_t file_position;
    int32_t flags;
} file_descriptor_t;

typedef struct pcb {
    file_descriptor_t fd_array[?];
} pcb_t;

#endif
