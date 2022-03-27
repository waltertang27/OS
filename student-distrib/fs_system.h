#include "lib.h"
#include "types.h"
#include "kernel.c"

#define MAX_FILE_NAME 32 
#define RESERVED_DIRENTRY_SPACE 24
#define RESERVED_BOOT_SPACE 52
#define NUM_DIR_ENTRIES 64
#define TOTAL_BLOCK_NUM 1024

//Global pointers to start of different sections 
boot_block_t * start_boot_block;
INode_t * start_INode ;
int32_t * start_data_block; 
dentry_t * directory_start ; 


typedef struct{
    uint8_t fileName[MAX_FILE_NAME]; 
    uint32_t fileType;
    uint32_t INodeNum;
    uint8_t reserved[RESERVED_DIRENTRY_SPACE];
} dentry_t; 

typedef struct{
    uint32_t num_dir_entries; 
    uint32_t inodes; 
    uint32_t data_blocks ;
    uint8_t reserved[RESERVED_BOOT_SPACE];
    dentry_t dir_entries[NUM_DIR_ENTRIES];
} boot_block_t; 

typedef struct 
{
    uint32_t b_length ;
    uint32_t block_num[TOTAL_BLOCK_NUM];
}INode_t;


// Initalize the filesystem
void FileSystem_Init(uint32_t * fs_start);


//Dont know what these do yet lol
int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry);
int32_t read_dentry_by_index(const uint8_t *index, dentry_t *dentry);


// Operations on files
int32_t file_open(void);
int32_t file_close(void);
int32_t file_read(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length);
int32_t file_write(void);

// Operations on directories
int32_t directory_read(uint32_t fd, void * buf, int32_t nbytes);
int32_t directory_write(void);
int32_t directory_open(void);
int32_t directory_close(void);

