#include "fs_system.h"

file_descriptor_t temp_global_array[64];

int32_t open(const uint8_t* filename, int fd){
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

   // directoryStart = startBootBlock->dirEntries;
    startINode = (INode_t *)(fs_start + FOURKB);
    startDataBlock = (uint32_t *)(startINode + startBootBlock->InodesNum);
    // printf("start I node Pointer %p ",startINode);
    // Initialize global variables that will be used for reads
    dentryIDX = 0; 
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
    int filledDentry = -1; 
    uint8_t *currName;

    
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
            filledDentry = 0; 
            break;
        }
    }

    return filledDentry; 
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
    // printf("FileName: %s, InodeNum: %u Bytes in each Inode %u \n",dentry->fileName,dentry->INodeNum,startINode[dentry->INodeNum].bLength);
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
    
    INode_t * curr_inode; 
    uint32_t * currBlock; 
    uint32_t bytes = 0;
    uint32_t temp, blockOffset, bytesToCopy, end_of_file; 
    uint32_t file_byte_size; 

    //uint32_t start, end, index; 
    // Get a pointer to the inode we are going to use using the inodeIDX 

    curr_inode = &startINode[inodeIdx];
    file_byte_size = curr_inode->bLength;

    end_of_file = 0; 

    // Calculate how many blocks and bytes you need to jump before starting your read
    temp = offset / FOURKB;
    blockOffset = offset % FOURKB;

    // Figure out the right starting block and where in that block to start 
    
    // currBlock = (int32_t *)(curr_inode->blockData[temp] + blockOffset) ;
    currBlock = (uint32_t * )(startDataBlock + FOURKB * temp + blockOffset);

    // Copy from the offset to the end of that block
    bytesToCopy = FOURKB - offset;

    //If you reach the limit for bytes you can copy before the entirety of a block
    if (bytesToCopy > length)
        bytesToCopy = length; 

    if(bytesToCopy > file_byte_size)
        bytesToCopy = file_byte_size; 
    

    memcpy(buf, currBlock, bytesToCopy) ; // This is definety wrong but you get what im trying to do
    bytes += bytesToCopy; 

    bytesToCopy = FOURKB; 
    
    while(bytes != length) // Add some condition to check if it is at the end of the file
    {
        // currBlock = curr_inode->blockData[++temp];
        temp++;
        currBlock = (uint32_t * )(startDataBlock + FOURKB * temp);

        if(bytesToCopy > length)
            bytesToCopy = length;

        // if bytesToCopy is greater  than how much we have in the file
        if (bytesToCopy > (file_byte_size - bytes)){
            bytesToCopy = file_byte_size - bytes;
            end_of_file = 1; 
        }

        memcpy(buf + bytes, currBlock, bytesToCopy);
        bytes += bytesToCopy; 
        
        if(end_of_file)
            break; 
    }

    return bytes;
    
}


// read file and fill buffer 
int32_t file_read(uint32_t fd, void *buf, int32_t nbytes){

    // read file
    open((uint8_t)&" ", fd);
    
    int32_t bytes = read_data(temp_global_array[fd].inode, temp_global_array[fd].file_position, buf, nbytes);
    return bytes;
}


// Read a directory entry and fill the buffer with the corresponding value
int32_t directory_read(uint32_t fd, void *buf, int32_t nbytes)
{ 
    
    dentry_t currDir; 
    int32_t error;


    // read into dentry
    error = read_dentry_by_index(temp_global_array[fd].file_position, &currDir);
	// printf(" Filename: %s, File Type: %d, File Size %d \n ", currDir.fileName, currDir.fileType, startINode[currDir.INodeNum].bLength);
    if (error == -1){
        return 0;
    }
    temp_global_array[fd].file_position++;
    // void * can be anything
    // strncpy: Copies the first num characters of source to destination.
    strncpy((int8_t * )buf, (int8_t * )&(currDir.fileName), nbytes);
    return nbytes;
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
