#ifndef SYSTEMCALLS_H
#define SYSTEMCALLS_H

#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"
#include "fs_system.h"
#include "paging.h"

#define STDIN 0
#define STDOUT 1
#define FD_START_INDEX 2 // file descriptor start index, not including stdin (0) or stdout (1).

#define MAGIC_1 0x7f
#define MAGIC_2 0x45
#define MAGIC_3 0x4c
#define MAGIC_4 0x46

#define SIZE_OF_INT32 4

#define IN_USE 0
#define FREE 1
#define FD_ARRAY_SIZE 8
#define PROCESS_ARRAY_SIZE 4
#define EIGHTMB 8388608 // 8MB
#define EIGHTKB 8192 // 8KB
#define PROCESS_ADDR 0x8048000

uint32_t process_array[PROCESS_ARRAY_SIZE];

// https://stackoverflow.com/questions/9932212/jump-table-examples-in-c
typedef struct fileop_jmp_table
{
    // Pointers to open functions(need to be initialized)
    int32_t (*open)(const uint8_t *filename);
    int32_t (*read)(int32_t fd, void *buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void *buf, int32_t nbytes);
    int32_t (*close)(int32_t fd);
} fileop_jmp_table_t;


typedef struct file_descriptor
{
    fileop_jmp_table_t *jump_table;
    int32_t inode;
    int32_t file_position;
    int32_t flags; // free or in use?
} file_descriptor_t;


typedef struct pcb {
    int32_t process_id;
    int32_t parent_id;
    int32_t save_ebp;
    int32_t save_esp;
    int32_t usr_eip;
    int32_t usr_esp;
    int32_t active;
    file_descriptor_t fd_array[FD_ARRAY_SIZE];
    int8_t pcb_cmd[32]; 
} pcb_t;


pcb_t * get_pcb(int32_t id);
pcb_t *get_cur_pbc();

int32_t halt (uint8_t status); //3.1
int32_t execute (const uint8_t* command); //3.1

void auto_open(int stdfile);

int32_t read (int32_t fd, void* buf, int32_t nbytes); //3.1
int32_t write (int32_t fd, const void* buf, int32_t nbytes); //3.1
int32_t open (const uint8_t* filename); //3.1
int32_t close (int32_t fd); //3.1

// If a file cant read, write, open, or close these their jump tables will be sent here 
int32_t read_fail(int32_t fd, void *buf, int32_t nbytes);
int32_t write_fail(int32_t fd, const void *buf, int32_t nbytes);
int32_t open_fail(const uint8_t *filename);
int32_t close_fail(int32_t fd);

int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);

void fileop_init(); 
void auto_open(int stdfile);
void auto_open(int stdfile);

//Corresponding open and close functions for each of the following file descriptors 
fileop_jmp_table_t stdin_fileop;
fileop_jmp_table_t stdout_fileop; 
fileop_jmp_table_t null_op; 


#endif

