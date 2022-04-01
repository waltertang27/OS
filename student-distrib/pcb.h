#ifndef PCB_H
#define PCB_H

#include "types.h"
#include "fs_system.h"

#define FD_ARRAY_SIZE 8
#define EIGHTMB 8388608 // 8MB
#define EIGHTKB 8192 // 8KB



typedef struct pcb {
    int32_t process_id;
    int32_t parent_id;
    file_descriptor_t fd_array[FD_ARRAY_SIZE];
} pcb_t;


pcb_t * get_pcb(int32_t id);
pcb_t *get_cur_pbc();

#endif

