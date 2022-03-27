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
    currdentry = directoryStart ;
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
    int i;
    uint8_t *currName;
    dentry_t currEntry; 
    
    int fileNameLength = strlen((int8_t *)fname);
    if(fileNameLength > MAX_FILE_NAME){
        return -1; 
        printf("Filename was too long");
    }

    for (i = 0; i < NUM_DIR_ENTRIES; i++){
        currName = startBootBlock->dirEntries[i].fileName;
        if (strlen((int8_t *)currName) != fileNameLength)
            continue;
        else if (strncmp((int8_t *)currName, (int8_t *)fname, sizeof(fname)))
            continue;
        else{
            // They should have the same name lol so ima copy the useful stuff 
            dentry->fileType = startBootBlock->dirEntries[i].fileType; 
            dentry->INodeNum = startBootBlock->dirEntries[i].INodeNum; 
            break;
        }
    }   
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
int32_t read_dentry_by_index(const uint8_t index, dentry_t *dentry)
{
    if (index > NUM_DIR_ENTRIES -1 )
        return -1;

    int8_t *currWord = (int8_t *)  startBootBlock->dirEntries[index].fileName;
    strcpy((int8_t *) dentry->fileName, currWord);

    dentry->fileType = startBootBlock->dirEntries[index].fileType;
    dentry->INodeNum = startBootBlock->dirEntries[index].INodeNum;


    return 0; 
}


/*
* The last routine works much like the read system call, reading up to
* length bytes starting from position offset in the file with inode number inode and returning the number of bytes
* read and placed in the buffer. A return value of 0 thus indicates that the end of the file has been reached
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length){

    // INode_t * curr_inode; 
    // uint32_t blockNum; 
    
    // int bytes = 0;
    // int i; 
    // curr_inode = (INode_t*)(startINode[inode]);

    // for(bytes = 0; bytes<length;bytes++){


    //     blockNum =  curr_inode->blockData[i + offset];

    // }



    return 0;
}


// read file and fill buffer 
int32_t file_read(uint32_t fd, void *buf, int32_t nbytes){


    // read file
    // int32_t bytes = read_data(/* where do we get the inode / offset? */, buf, nbytes);
    // return bytes;
    return 0; 
}


// Read a directory entry and fill the buffer with the corresponding value
int32_t directory_read(uint32_t fd, void *buf, int32_t nbytes)
{
    // dentry_t dentry;   
    // int32_t error, bytes;

    // // read into dentry
    // error = read_dentry_by_index(count, &dentry);
    // if (error == -1){
    //     return 0;
    // }

    // // void * can be anything
    // // strncpy: Copies the first num characters of source to destination.
    // strncpy((int8_t * )buf, (int8_t * )&(dentry.fileName), MAX_FILE_NAME);
    // bytes = strlen((int8_t*)&(dentry.fileName));
    // return bytes;
    return 0; 
}




// Apparently since its a read only file system these dont matter and should all return 0 

// I think this is -1, lab slides say so
int32_t file_write(){
    return -1; 
}

int32_t directory_write()
{
    return -1;
}

int32_t file_open(){
    return 0; 
}

int32_t directory_open(){
    return 0; 
}

int32_t file_close(){
    return 0; 
}

int32_t directory_close()
{
    return 0;
}

