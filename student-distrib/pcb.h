#ifndef PCB_H
#define PCB_H

#define FD_ARRAY_SIZE 8
#define EIGHTMB 8388608 // 8MB
#define EIGHTKB 8192 // 8KB

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
    int32_t flags; // free or in use?
} file_descriptor_t;

typedef struct pcb {
    int32_t process_id;
    int32_t parent_id;
    file_descriptor_t fd_array[FD_ARRAY_SIZE];
} pcb_t;


pcb_t * get_pcb(int32_t id);
pcb_t *get_cur_pbc();

#endif