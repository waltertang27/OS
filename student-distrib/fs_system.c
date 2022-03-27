#include "fs_system.h"



/*
FileSystem_Init
DESCRIPTION: Initializes the Pointers necessary to preform operations on the file system  
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: startBootBloc, diectoryStart, startINode, and startDataBlock will have adresses filled in 
*/
void FileSystem_Init(uint32_t *fs_start){
    //Initialize all pointers for our file system when given the starting address of the file system 
    startBootBlock  = (boot_block_t * )fs_start;
    directoryStart = startBootBlock->dirEntries;
    startINode = (INode_t *)(startBootBlock + 1);
    startDataBlock = (uint32_t *)(startINode + startBootBlock->InodesNum);
}



/*
read_dentry_by_name
DESCRIPTION: Read a directory entry when given its name 
INPUTS: 
    fname - A maximum on 32 letters which will be searched for
    dentry - A directory entry  that will be filled in 
RETURN VALUE: 0 on sucess 1 on failure 
SIDE EFFECTS: dentry will be filled with the correct information if the file exists 
*/
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry)
{
    return 0; 
}




/*
read_dentry_by_index
DESCRIPTION: Read a directory entry when given its index within all directories  
INPUTS: 
    fname - A index which cannot be larger than 64 that corresponds to a dir entry 
    dentry - A directory entry  that will be filled in 
RETURN VALUE: 0 on sucess 1 on failure 
SIDE EFFECTS: dentry will be filled with the correct information if the file exists 
*/
int32_t read_dentry_by_index(const uint8_t *index, dentry_t *dentry)
{
    return 0; 
}

int32_t directory_read(uint32_t fd, void *buf, int32_t nbytes)
{
    return 0; 
}

int32_t file_read(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length){
    return 0; 
}

