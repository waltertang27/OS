#include "fs_system.h"

void FileSystem_Init(uint32_t *fs_start){
    //cast it into a pointer of boot block type since the first block in filesystem is the boot block
    startBootBlock  = (boot_block_t * )file_system_start;
    directoryStart = startBootBlock->dir_entries;
    startINode = (INode_t *)(startBootBlock + 1);
    startDataBlock = (uint32_t *)(startINode + startBootBlock->INodeNum);
}

int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry)
{
}
int32_t read_dentry_by_index(const uint8_t *index, dentry_t *dentry)
{
}

int32_t directory_read(uint32_t fd, void *buf, int32_t nbytes)
{
}

int32_t file_read(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length){

}

