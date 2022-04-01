//  to do in 6.3.1
#include "syscall.h"

#define MAX_CMD_LINE_SIZE 32 // not sure
#define BUF_SIZE 4

int32_t id = 0;
int32_t curr_id = 0;

int32_t halt(uint8_t status){
    /* The halt system call terminates a process, returning the specified value to its parent process. The system call handler
    itself is responsible for expanding the 8-bit argument from BL into the 32-bit return value to the parent program’s
    execute system call. Be careful not to return all 32 bits from EBX. This call should never return to the caller. */

    /*
        Restore parent data
        Restore parent paging
        Close any relevant FDs
        Jump to execute return
    */

    asm volatile (" 
        leave ;
        ret ;
    "
    );

    // fail
    return -1;
}

int32_t execute (const uint8_t* command){
    /* The execute system call attempts to load and execute a new program, handing off the processor to the new program
        until it terminates. The command is a space-separated sequence of words. The first word is the file name of the
        program to be executed, and the rest of the command—stripped of leading spaces—should be provided to the new
        program on request via the getargs system call. The execute call returns -1 if the command cannot be executed,
        for example, if the program does not exist or the filename specified is not an executable, 256 if the program dies by an
        exception, or a value in the range 0 to 255 if the program executes a halt system call, in which case the value returned
        is that given by the program’s call to halt. -- appendix B */

    /*
        Parse args
        Check for executable
        Set up paging
        Load file into memory
        Create PCB
        Prepare for Context Switch
        Push IRET context to kernel stack
    */

    /* parsing */
    int command_size = strlen( (const uint8_t * ) command);
    int i = 0;
    int spaces;
    uint8_t cmd[MAX_CMD_LINE_SIZE]; // again, size not sure
    uint8_t args[MAX_CMD_LINE_SIZE];
    uint8_t buf[BUF_SIZE];
    uint8_t * memory;
    // uint8_t * inode;
    dentry_t dentry;
    pcb_t * pcb;

    /* 
        Part 1: Parsing command and args.
        cmd: read spaces until we have read a non space character. Then write the non spaces 
        characters to cmd until we read a space character.
        If all spaces, return fail.
    */
    while (command[i] == ' '){
        if (i == command_size - 1){
            return -1;
        }
        i++;
    }
    spaces = i;
    while (command[i] != ' '){
        cmd[i - spaces] = command[i];
        i++;
    }

    /* args */

    // check logic
    while (i < command_size){
        while (command[i] == ' '){
            if (i == command_size - 1){
                break;
            }
            i++;
            spaces++;
        }
        while (command[i] != ' '){
            args[i - spaces] = command[i];
            i++;
        }
        if (i != command_size - 1){
            args[i - spaces] = ' ';
        }
    }

    /* check for executable */

    /* about magic nums: The first 4 bytes of the file represent a “magic number” that identifies the file as an executable. 
        These bytes are, respectively, 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46. If the magic number is not present, the execute
        system call should fail. -- appendix C*/

    if (read_dentry_by_name(cmd, &dentry)  == -1) { // no file
        return -1;
    } else if (read_data(dentry.inode_num, 0, buf, BUF_SIZE) == -1){ // data reading failure
        return -1;
    } else if (buf[0] != MAGIC_1){ // check for magic nums
        return -1;
    } else if (buf[1] != MAGIC_2){
        return -1;
    } else if (buf[2] != MAGIC_3){
        return -1;
    } else if (buf[3] != MAGIC_4){
        return -1;
    }

    /* Set up paging */

    // what

    /* Load file into memory */
    read_data(dentry.inode_num, 0, memory, ???);

    /* Create PCB */
    pcb = get_pcb(id);
    pcb->process_id = id;
    curr_id = id;
    id++;

    auto_open(STDIN);
    auto_open(STDOUT);

    // remaining six file descriptors available
    for (i = FD_START_INDEX; i < FD_ARRAY_SIZE i++) {
        // pcb->fd_array[i].jump_position = ???;     
        // pcb->fd_array[i].file_position = ???;
        pcb->fd_array[i].flags = FREE;
        // pcb->fd_array[i].inode = ???;
    }

    /* Prepare for Context Switch */

    // wtf is context switch

    /* Push IRET context to kernel stack */
    asm volatile ("
        iret ;
    "
    );
    return 172;  // value between 0 and 255
}

void auto_open(int stdfile){
    pcb = get_pcb(curr_id);

    // pcb->fd_array[stdfile].jump_position = ???;     
    // pcb->fd_array[stdfile].file_position = ???;
    pcb->fd_array[stdfile].flags = IN_USE;
    // pcb->fd_array[stdfile].inode = ???;
    return;
}


int32_t read (int32 t fd, void* buf, int32_t nbytes){
    // fail
    return -1;
}

int32_t write (int32 t fd, const void* buf, int32_t nbytes){
    /*
        The write system call writes data to the terminal or to a device (RTC). In the case of the terminal, all data should
        be displayed to the screen immediately. In the case of the RTC, the system call should always accept only a 4-byte
        integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly. Writes to regular
        files should always return -1 to indicate failure since the file system is read-only. The call returns the number of bytes
        written, or -1 on failure.

    */ 
    // fail
    return -1;
}

int32_t open (const uint8_t* filename){
    /* The open system call provides access to the file system. The call should find the directory entry corresponding to the
    named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
    RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1. */

    dentry_t dentry;
    int error;

    // check if filename is valid
    if (filename == NULL){
        return -1;
    }

    // read into dentry
    error = read_dentry_by_name(filename, &dentry);
    if (error == -1){
        return -1;
    }
    
    for(i = FD_START_INDEX; i < ???; i++) {

    }

    
    
    // fail
    return -1;
}

int32_t close (int32_t fd){
    if (fd == STDIN || fd == STDOUT){
        return -1;
    }

    // fail
    return -1;
}
