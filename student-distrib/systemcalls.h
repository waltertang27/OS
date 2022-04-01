#ifndef SYSTEMCALLS_H
#define SYSTEMCALLS_H

#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "pcb.h"

#define STDIN 0
#define STDOUT 1
#define FD_START_INDEX 2 // file descriptor start index, not including stdin (0) or stdout (1).

#define MAGIC_1 0x7f
#define MAGIC_2 0x45
#define MAGIC_3 0x4c
#define MAGIC_4 0x46

#define EIGHTMB 4194304 * 2 // 8MB
#define EIGHTKB 8192 // 8KB

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


#endif
