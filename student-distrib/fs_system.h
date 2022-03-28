#include "lib.h"
#include "types.h"

#define MAX_FILE_NAME 32 
#define RESERVED_DIRENTRY_SPACE 24
#define RESERVED_BOOT_SPACE 52
#define NUM_DIR_ENTRIES 64
#define TOTAL_BLOCK_NUM 1023
#define FOURKB 4096
//Global pointers to start of different sections 



typedef struct dentry {
    uint8_t fileName[MAX_FILE_NAME]; 
    uint32_t fileType;
    uint32_t INodeNum;
    uint8_t reserved[RESERVED_DIRENTRY_SPACE];
} dentry_t; 

typedef struct boot_block{
    uint32_t numDirEntries; 
    uint32_t InodesNum; 
    uint32_t dataBlocks ;
    uint8_t reserved[RESERVED_BOOT_SPACE];
    dentry_t dirEntries[NUM_DIR_ENTRIES];
} boot_block_t; 

typedef struct INode {
    uint32_t bLength ;
    uint32_t blockData[TOTAL_BLOCK_NUM];
}INode_t;


boot_block_t * startBootBlock;
INode_t * startINode ;
uint32_t * startDataBlock; 
dentry_t * directoryStart ; 

//File position is how many bytes of the current dentry have been read 
uint32_t filePosition; 

//The current file that is being read(0 is the first file, 1 is the second)
uint32_t dentryIDX ; 

// Initalize the filesystem
void FileSystem_Init(uint32_t * fsStart);


//Dont know what these do yet lol
int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry);
int32_t read_dentry_by_index(const uint8_t index, dentry_t * dentry);


// Operations on files
int32_t file_open(void);
int32_t file_close(void);
int32_t file_read(uint32_t fd, void *buf, int32_t nbytes);
int32_t file_write(void);

// Operations on directories
int32_t directory_read(uint32_t fd, void * buf, int32_t nbytes);
int32_t directory_write(void);
int32_t directory_open(void);
int32_t directory_close(void);

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length);

// temporary open
int32_t open(const uint8_t* filename, int fd);

typedef struct file_descriptor {
    int32_t inode;
    int32_t file_position;
    int32_t flags;
} file_descriptor_t;

