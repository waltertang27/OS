#include "lib.h"
#include "types.h"

#define MAX_FILE_NAME 32 
#define RESERVED_DIRENTRY_SPACE 24
#define RESERVED_BOOT_SPACE 52
#define NUM_DIR_ENTRIES 64


//Global pointers to start of different sections 
int32_t * start_boot_block;
int32_t * start_INode ;
int32_t * start_data_block; 


typedef struct{
    // 32 bytes(each one used for an ascii character)
    uint8_t fileName[MAX_FILE_NAME]; 
    uint32_t fileType;
    uint32_t INodeNum;
    // 24 bytes
    uint8_t reserved[RESERVED_DIRENTRY_SPACE];
} dentry_t; 

typedef struct{
    uint32_t num_dir_entries; 
    uint32_t inodes; 
    uint32_t data_blocks ;
    uint8_t reserved[52] ;
    uint8_t dir_entries[64] ;
} boot_block_t; 

typedef struct 
{
    uint32_t b_length ;
    // Not sure how many blocks ther are 
    uint32_t block_num[100] ; 
}INode;


// Initalize the filesystem
void FileSystem_Init(uint32_t * fs_start);


//Dont know what these do yet lol
int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry);
int32_t read_dentry_by_index(const uint8_t *index, dentry_t *dentry);


// Operations on files
int32_t file_open(void);
int32_t file_close(void);
int32_t file_read(void);
int32_t file_write(void);

// Operations on directories
int32_t directory_read(void);
int32_t directory_write(void);
int32_t directory_open(void);
int32_t directory_close(void);