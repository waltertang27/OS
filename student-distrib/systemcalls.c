// to do in 6.3.1
<<<<<<< HEAD
=======

// #include "syscall.h"

// #define STDIN 0
// #define STDOUT 1
// #define FD_START_INDEX 2 // file descriptor start index, not including stdin (0) or stdout (1).

// int32_t halt(uint8_t status){
//     /* The halt system call terminates a process, returning the specified value to its parent process. The system call handler
//     itself is responsible for expanding the 8-bit argument from BL into the 32-bit return value to the parent program’s
//     execute system call. Be careful not to return all 32 bits from EBX. This call should never return to the caller. */


//     asm volatile (" 
//         leave               \n\
//         ret                 \n\
//     "
//     );

//     // fail
//     return -1;
// }

// int32_t execute (const uint8_t* command){
//     // fail
//     return -1;
// }

// int32_t read (int32 t fd, void* buf, int32_t nbytes){
//     // fail
//     return -1;
// }

// int32_t write (int32 t fd, const void* buf, int32_t nbytes){
//     // fail
//     return -1;
// }

// int32_t open (const uint8_t* filename){
//     /* The open system call provides access to the file system. The call should find the directory entry corresponding to the
//     named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
//     RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1. */

//     dentry_t dentry;
//     int error;

//     // check if filename is valid
//     if (strlen((char*)filename) == NULL){
//         return -1;
//     }

//     // read into dentry
//     error = read_dentry_by_name(fname, &dentry);
//     if (error == -1){
//         return -1;
//     }
    
//     for(i = FD_START_INDEX; i < ???; i++) {

//     }


    
//     // fail
//     return -1;
// }

// int32_t close (int32_t fd){
//     if (fd == STDIN || fd == STDOUT){
//         return -1;
//     }

//     // fail
//     return -1;
// }
>>>>>>> 11b68a76ebfb87ae7bbd047ab5e5930d2ae6fa0d
