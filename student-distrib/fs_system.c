#include "fs_system.h"

file_descriptor_t temp_global_array[64];

int32_t open_(const uint8_t* filename, int fd){
    temp_global_array[fd].file_position = 0;
    return fd;
}

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

    //directoryStart[0] would give the first entry 
    directoryStart = startBootBlock->dirEntries;

    //The block that follows the boot block is 4KB after it 
    startINode = (INode_t *)(startBootBlock + 1);

    //Add the number of inode from the starting pointer to get a pointer to the first block 
    startDataBlock = (int8_t *)(startINode + startBootBlock->InodesNum);

    // Initialize global variables that will be used for reads
    currDentry = directoryStart[0] ;
    filePosition = 0;
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
    // int filledDentry = -1; 
    uint8_t currName[32];
    uint8_t concat[32]; 
    
    memset((void*)concat,0,32); 

    int fileNameLength = strlen((int8_t *)fname);
    // printf("%s length: %d \n", fname,fileNameLength);

    strncpy((void *)concat,(const void*)fname,32);
    
    if(fileNameLength > MAX_FILE_NAME){
        printf("Filename was too long concatonated to %s \n",concat);
        fileNameLength = 32; 
    }

    if(dentry == NULL || fname == NULL)
        return -1; 


    for (i = 0; i < NUM_DIR_ENTRIES; i++){
        memset((void*)currName,0,32); 
        strncpy((int8_t *)currName,(int8_t *)directoryStart[i].fileName,32); 

        if (strncmp((int8_t *)currName, (int8_t *)concat, 32)){
            continue;
        }
        else{
           // printf("Else i: %d, currname: %s \n",i,currName);
            dentry->fileType = directoryStart[i].fileType;
            dentry->INodeNum = directoryStart[i].INodeNum;
            strncpy((int8_t *)dentry->fileName, (int8_t *)currName,32);
            return 0 ;
        }
    }
    return -1; 
    
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
    uint8_t concat[32]; 
    memset((void*)concat,0,32); 

    if (index > NUM_DIR_ENTRIES -1)
        return -1;

    if(dentry == NULL)
        return -1; 

    strncpy((int8_t *)concat,(const int8_t*)directoryStart[index].fileName,32);

    strncpy((int8_t *) dentry->fileName, (int8_t *)concat,32);

    dentry->fileType = directoryStart[index].fileType;
    dentry->INodeNum = directoryStart[index].INodeNum;

  //  printf("   FileName: %s, InodeNum: %u Bytes in each Inode %u    ",dentry->fileName,dentry->INodeNum,startINode[dentry->INodeNum].bLength);

    return 0; 
}

/*
read_data
DESCRIPTION: Read a data from an inode and fills a buffer 
INPUTS: 
    inocdeIdx - An index into the inodes(1 would get the first inode, 2 would get second)
    Offset - The byte number that you want to start at from the begining of a file
    buf - Buffer which will be filled will the contents of the read
    length - the maximum number of bytes to be read 
RETURN VALUE: 0 on sucess 1 on failure 
SIDE EFFECTS: From the given index and offset, the buffer is filled with bytes until length is reached or end of file
*/
int32_t read_data(uint32_t inodeIdx, uint32_t offset, uint8_t *buf, uint32_t length){
    
    INode_t * curr_inode = startINode + inodeIdx;   //Pointer to the start of the block we will be copying 
    uint32_t bytes = 0;                             //Running total of bytes copied 
    uint32_t file_byte_size = curr_inode->bLength;  //The total size of the file you are copying in bytes
    uint32_t blocksSkipped = offset / FOURKB;       //How many blocks to skip from the starting block
    uint32_t blockOffset = offset % FOURKB;         //After skippin blocks how many bytes need to be skipped 
    uint32_t end_of_file = 0;                       //Flag to stop copying 
    void * dataBlock  = (void*)startDataBlock;      //Void pointer to start of the data block

    uint32_t blockIDX = curr_inode->blockData[blocksSkipped];               //The index of the start block
    uint32_t bytesToCopy = FOURKB - blockOffset;                            //The amount of bytes to copy to go from the offset to the end of the block 
    void*  currBlock = (dataBlock + ((blockIDX ) * FOURKB) + offset);          //Pointer to the first byte in the block INCLUDING the offset 


    if(blocksSkipped * FOURKB + blockOffset > file_byte_size){
        // printf("Skipped: %u offset: %u \n",blocksSkipped,blockOffset); 
        return 0; 
    }
    

    //If you reach the limit for bytes you can copy before the entirety of a block
    if (bytesToCopy > length) {
        bytesToCopy = length; 
        end_of_file = 1;
    }  

    if(bytesToCopy > file_byte_size - offset){
        bytesToCopy = file_byte_size - offset; 
        end_of_file = 1; 
    }

    if(end_of_file){
        end_of_file = 0; 
        memcpy((void *)buf, (const void*) currBlock , bytesToCopy) ; 
        bytes += bytesToCopy; 
        return bytes; 
    }

    //If you are reading more than 4kb 
    memcpy((void *)buf, (const void*)currBlock, bytesToCopy) ; 
    bytes += bytesToCopy; 

    bytesToCopy = FOURKB; 
    
    while(bytes < length) // While you have not copied all the bytes
    {
       // printf("Entered while loop with bytes: %u \n",bytes);
        blocksSkipped++;
        blockIDX = curr_inode->blockData[blocksSkipped];    
        currBlock = (dataBlock + (blockIDX  * FOURKB) );   

        //If 4k is larger than the file or what you have left
        if (bytesToCopy > length - bytes ){
            bytesToCopy = length - bytes; 
            end_of_file = 1;
        }  

        if(bytesToCopy > file_byte_size - bytes - offset){
            bytesToCopy = file_byte_size - bytes - offset ; 
            end_of_file = 1; 
        }

        if(end_of_file){
            end_of_file = 0; 
            memcpy(buf + bytes, currBlock + offset, bytesToCopy) ; 
            bytes += bytesToCopy; 
            return bytes; 
        }

        memcpy(buf + bytes, currBlock, bytesToCopy);
        bytes += bytesToCopy; 
    }

    return bytes;
    
}


// read file and fill buffer 
int32_t file_read(int32_t fd, void *buf, int32_t nbytes){

    // read file
    pcb_t * curr = get_cur_pcb(); 
    file_descriptor_t  * array =  curr->fd_array ;
    int32_t filesize = startINode[curr->fd_array[fd].inode].bLength; 
    

    int32_t bytes = read_data(curr->fd_array[fd].inode, array[fd].file_position, buf, nbytes);
    if(bytes == -1)
        return -1; 

    // printf("%s \n",buf);
    array[fd].file_position += bytes; 
    
    return bytes;
}


// Read a directory entry and fill the buffer with the corresponding value
int32_t directory_read(int32_t fd, void *buf, int32_t nbytes)
{

    dentry_t currDir; 
    int32_t error;

    pcb_t * curr = get_cur_pcb();

    // read into dentry
    // printf("%d \n ",curr->fd_array[fd].file_position);
    error = read_dentry_by_index(curr->fd_array[fd].file_position, &currDir);

    if (error == -1){
        return -1;
    }

    curr->fd_array[fd].file_position++;



    // void * can be anything
    // strncpy: Copies the first num characters of source to destination.
    

    memcpy((void * )buf, (const void * )((&currDir)), nbytes);
    if(curr->fd_array[fd].file_position == 18)
        return 0; 

    return nbytes;
}




// Apparently since its a read only file system these dont matter and should all return 0 

// I think this is -1, lab slides say so
int32_t file_write(int32_t fd, const void *buf, int32_t nbytes){
    return -1; 
}

int32_t directory_write(int32_t fd, const void *buf, int32_t nbytes)
{
    return -1;
}

int32_t file_open(const uint8_t *filename){
    return 0; 
}

int32_t directory_open(const uint8_t *filename){
    return 0; 
}

int32_t file_close(int32_t fd){
    return 0; 
}

int32_t directory_close(int32_t fd)
{
    return 0;
}
