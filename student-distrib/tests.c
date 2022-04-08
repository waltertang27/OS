#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "idt.h"
#include "rtc.h"
#include "fs_system.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER \
    printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result) \
    printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure()
{
    /* Use exception #15 for assertions, otherwise
       reserved by Intel */
    asm volatile("int $15");
}

/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test()
{
    TEST_HEADER;

    int i;
    int result = PASS;
    for (i = 0; i < 10; ++i)
    {
        if ((idt[i].offset_15_00 == NULL) &&
            (idt[i].offset_31_16 == NULL))
        {
            assertion_failure();
            result = FAIL;
        }
    }

    return result;
}

// add more tests here

/* division_by_zero_test 
 *
 * Cause blue screen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Halts the OS and displays errors
 * Coverage: Exception handling
 * Files: idt.c
 */
int division_by_zero_test()
{
    TEST_HEADER;
    int a = 0;
    int b = 1 / a;
    b = b + 1; // avoid warning
    return FAIL;
}

/* syscall_test()
 *
 * Cause blue screen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Halts the OS and displays errors
 * Coverage: Exception handling
 * Files: idt.c
 */
int syscall_test()
{
    TEST_HEADER;
    __asm__("int    $0x80");
    return FAIL;
}

/* paging_init_test()
 *
 * Initialize paging
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Halts the OS and displays errors
 * Coverage: Kernel index of page directory from paging
 * Files: paging.c
 */
int paging_init_test()
{
    TEST_HEADER;

    if (page_directory[0].present != 1 || page_directory[KERNEL_INDEX].present != 1){
        return FAIL;
    }

    return PASS;
}

/* paging_test()
 *
 * Dereference the kernel and video memory addresses 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Halts the OS and displays errors
 * Coverage: Kernel and video memory addresses from paging
 * Files: paging.c
 */
int paging_test()
{
    TEST_HEADER;

    char ref;
    char * temp;

    char *pointer = (char *)0x400000; // kernel
    ref = *pointer;

    pointer = (char *)0x0B8000; // video memory
    ref = *pointer;

    pointer = (char *)0x7FFFFF;
    ref = *pointer;

    return PASS;
}

/* null_test()
*
* Cause blue screen if dereferencing 0 causes a page fault
* Inputs: None
* Outputs: Page fault/FAIL
* Side Effects: Halts the OS and displays errors
* Coverage: page fault handling of location 0.
* Files: paging.c
 */
int null_test()
{
    TEST_HEADER;
    char ref;
    char * temp;

    // convert 0 to char pointer for NULL

    char *pointer = (char *)0;
    ref = *pointer;
    return FAIL;
}

/* kernel_up_bound_test()
 *
 * Checks if there is a page fault from memory before the kernel
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side Effects: Halts the OS and displays errors
 * Coverage: page fault handling from memory before the kernel
 * Files: paging.c
 */
int kernel_up_bound_test()
{
    TEST_HEADER;
    char ref;
    char *pointer = (char *)0x3FFFFF;
    ref = *pointer;
    return FAIL;
}

/* kernel_low_bound_test()
 *
 * Checks if there is a page fault from memory after the kernel
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side Effects: Halts the OS and displays errors
 * Coverage: page fault handling from memory after the kernel
 * Files: paging.c
 */
int kernel_low_bound_test()
{
    TEST_HEADER;
    char ref;
    char *pointer = (char *)0x800000;
    ref = *pointer;
    return FAIL;
}
/* vidmem_up_bound_test()
 *
 * Checks if there is a page fault from memory before the video memory
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side Effects: Halts the OS and displays errors
 * Coverage: page fault handling from memory before the video memory
 * Files: paging.c
 */
int vidmem_up_bound_test()
{
    TEST_HEADER;
    char ref;
    char *pointer = (char *)0x0B7FFF;
    ref = *pointer;
    return FAIL;
}
/* vidmem_low_bound_test()
 *
 * Checks if there is a page fault from memory after the video memory
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side Effects: Halts the OS and displays errors
 * Coverage: page fault handling from memory after the video memory
 * Files: paging.c
 */
int vidmem_low_bound_test()
{
    TEST_HEADER;
    char ref;
    char *pointer = (char *)0x0B9000;
    ref = *pointer;
    return FAIL;
}


/* Checkpoint 2 tests */

/* Name Search Test
 *
 * Tests that the read_dentry_by_name function works, checking
 *      if the filename stored in the dentry is correct
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read_dentry_by_name() in fs_system.c
 */
int name_search_test(){
    TEST_HEADER;
    dentry_t entry; 
    uint8_t * word = (uint8_t * )"cat";
    int code; 
    code = read_dentry_by_name(word, &entry) ;
    
    if ((code == -1) || (strncmp((int8_t *)entry.fileName, (int8_t *)"cat", sizeof("cat"))))
    {
        return FAIL;
    }

    if ((code == -1) || (strncmp((int8_t *)entry.fileName, (int8_t *)"cat", sizeof("cat"))))
    {
        return FAIL;
    }

    word = (uint8_t *)"fish";
    code = read_dentry_by_name(word, &entry);

    if ((code == -1) || (strncmp((int8_t *)entry.fileName, (int8_t *)"fish", sizeof("fish"))))
    {
        return FAIL;
    }

    word = (uint8_t *)"frame0.txt";
    code = read_dentry_by_name(word, &entry);

    if ((code == -1) || (strncmp((int8_t *)entry.fileName, (int8_t *)"frame0.txt", sizeof("frame0.txt"))))
    {
        return FAIL;
    }
    

    word = (uint8_t *)"FFFF";
    if (read_dentry_by_name(word, &entry) != -1){
        return FAIL;
    }
    
    if(!read_dentry_by_name(word,NULL))
        return FAIL;

    if (!read_dentry_by_name((uint8_t *)" ", &entry))
        return FAIL;

    return PASS;
}

/* Index Search Test
 *
 * Tests that the read_dentry_by_index function works, checking
 *      if the filename stored in the dentry is correct
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read_dentry_by_index() in fs_system.c
 */
int idx_search_test(){
    TEST_HEADER;
    dentry_t entry, entry_2; 
    if (read_dentry_by_index(5, &entry) == -1){
        return FAIL;
    }

    if(strncmp((int8_t * ) entry.fileName, (int8_t * )"rtc",sizeof("rtc")))
        return FAIL; 
    

    if (read_dentry_by_index(10, &entry) == -1){
        return FAIL;
    }

    if(strncmp((int8_t * ) entry.fileName, (int8_t * )"frame0.txt",sizeof("frame0.txt")))
        return FAIL; 
    
    // printf("  %s  found at inode %u with file size %u \n", entry.fileName, entry.INodeNum, startINode[entry.INodeNum]); 

    if (read_dentry_by_index(2, &entry_2) == -1){
        return FAIL;
    }
    if(! strncmp((int8_t * ) entry_2.fileName, (int8_t * )"grep",sizeof("grep")))
        return FAIL; 

    if(!read_dentry_by_index(63,&entry))
        return FAIL; 

    if(!read_dentry_by_index(4,NULL))
        return FAIL;

    if (!read_dentry_by_index(-5, &entry))
        return FAIL;

    return PASS;
}

/* Long Text Test
 *
 * Tests that the read_dentry_by_name function works specifically
 *      for files with long names
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read_dentry_by_name() in fs_system.c
 */
int long_text_test(){
    TEST_HEADER;

    //  Keeps failing test 
   uint8_t* word = (uint8_t *)"verylargetextwithverylongname.txt";
   dentry_t entry; 
   int code = read_dentry_by_name(word, &entry);
    // printf("Name: %s \n",entry.fileName); 
    if ((code == -1) || (strncmp((int8_t *)entry.fileName, (int8_t *)"verylargetextwithverylongname.tx", 32)))
    {
        return FAIL;
    }
    return PASS;

}

/* Directory Read Test
 *
 * Tests that the directory read function works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints out the file name, type, and size for each file
 * Coverage: directory_read() in fs_system.c
 */
int directory_read_test(){
    TEST_HEADER;
	int i; 
    dentry_t dir_name;
    //Start at file 0 
    open_((uint8_t *)" ", 2);

	for(i = 0; i < 17; i++){
		directory_read(0, (dentry_t *)&dir_name, 54);
		if(!strlen((int8_t *) dir_name.fileName))
			return FAIL;
		else{
		    printf(" Filename: %s, File Type: %d, File Size %d \n ", dir_name.fileName,dir_name.fileType, startINode[dir_name.INodeNum].bLength);
		}
	}
    return PASS; 
}

/* Read Data Test
 *
 * Tests that the read data function works for cases without offsets
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints out the passed tests
 * Coverage: read_data() in fs_system.c
 */
int read_data_test_no_offset(){
    TEST_HEADER;
    dentry_t curr;
    //Create a buffer of 300 bytes and set everything equal to 0 
    uint8_t DataBuf[300] ; 
    memset(DataBuf,0,300);

    //Index 10 is frame0.txt 
    read_dentry_by_index(10,&curr);

    //printf("Name: %s, size %u \n",curr.fileName,startINode[curr.INodeNum].bLength); 

    if(read_data(curr.INodeNum, 0, DataBuf, 300) != 187){
        return FAIL;
    }
    else{
        printf(" Buffer: \n %s \n",DataBuf);
        printf("Test 1 passed \n");
    }


    read_dentry_by_name((uint8_t*)"grep",&curr);

    if(read_data(curr.INodeNum,0,DataBuf,300) != 300){

        return FAIL;
    }
    else{
        // printf("Buffer: \n");
        // for(i = 0; i<300; i++)
        //     if (DataBuf[i] != '\0')
        //         putc(DataBuf[i]);
        printf("\n");
        printf("Test 2 passed \n");
    }

    //Create a buffer of 300 bytes and set everything equal to 0 
    uint8_t DataBuf2[5000] ; 
    memset(DataBuf2,0,5000);  
    read_dentry_by_name((uint8_t*)"grep",&curr);
   // printf("File found: %s Size:%u \n",curr.fileName,startINode[curr.INodeNum].bLength);
    
    if(read_data(curr.INodeNum,0,DataBuf2,5000) != 5000){
        return FAIL;
    }
    else{
        // printf("Buffer: \n");
        // for(i = 0; i<5000; i++)
        //     if (DataBuf2[i] != '\0')
        //         putc(DataBuf2[i]);
       printf("Test 3 passed \n");
    }



    uint8_t DataBuf3[7000] ; 
    memset(DataBuf3,0,7000);  
    read_dentry_by_name((uint8_t*)"grep",&curr);
    // printf("File found: %s Size:%u \n",curr.fileName,startINode[curr.INodeNum].bLength);
    
    if(read_data(curr.INodeNum,0,DataBuf3,7000) != 6149){
        return FAIL;
    }
    else{
        // printf("Buffer: \n");
        // for(i = 0; i<6149; i++)
        //     if (DataBuf3[i] != '\0')
        //         putc(DataBuf3[i]);
        printf("\n");
        printf("Test 4 passed \n");
    }
    return PASS;   
}

/* Read Data Test
 *
 * Tests that the read data function works for cases with offsets
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints out the passed tests
 * Coverage: read_data() in fs_system.c
 */
int read_data_test_with_offset(){
    TEST_HEADER;
    dentry_t curr;
    //Create a buffer of 300 bytes and set everything equal to 0 
    uint8_t DataBuf[300] ; 
    memset(DataBuf,0,300);

    //Index 10 is frame0.txt 
    read_dentry_by_index(10,&curr);

    // printf("Name: %s, size %u \n",curr.fileName,startINode[curr.INodeNum].bLength); 

    if(read_data(curr.INodeNum, 100, DataBuf, 300) != 87){
        return FAIL;
    }
    else{
        // printf(" Buffer: \n %s \n",DataBuf);
        printf("Test 1 Passed \n");
    }
    

    read_dentry_by_name((const uint8_t*)"grep",&curr);
    memset(DataBuf,0,300);

    if(read_data(curr.INodeNum,50,DataBuf,300) != 300){
        return FAIL;
    }
    else{
        // printf("Buffer: \n");
        // for(i = 0; i<300; i++)
        //     if (DataBuf[i] != '\0')
        //         putc(DataBuf[i]);
        printf("\n");
        printf("Test 2 Passed \n");

    }

    //Create a buffer of 300 bytes and set everything equal to 0 
    uint8_t DataBuf2[5000] ; 
    memset(DataBuf2,0,5000);  
    read_dentry_by_name((const uint8_t*)"grep",&curr);
    printf("File found: %s Size:%u \n",curr.fileName,startINode[curr.INodeNum].bLength);
    
    if(read_data(curr.INodeNum,2000,DataBuf2,5000) != 6149 - 2000){
        return FAIL;
    }
    else{
        // printf("Buffer: \n");
        // for(i = 0; i<5000; i++)
        //     if (DataBuf2[i] != '\0')
        //         putc(DataBuf2[i]);
        printf("Test 3 Passed \n");

    }


    uint8_t DataBuf3[3000] ; 
    memset(DataBuf3,0,3000);  
    read_dentry_by_name((const uint8_t*)"grep",&curr);
   // printf("File found: %s Size:%u \n",curr.fileName,startINode[curr.INodeNum].bLength);
    
    if(read_data(curr.INodeNum,5000,DataBuf3,3000) != 1149){
        printf("%d \n", read_data(curr.INodeNum,5000,DataBuf3,3000));
        return FAIL;
    }
    else{
        // printf("Buffer: \n");
        // for(i = 0; i<6149; i++)
        //     if (DataBuf3[i] != '\0')
        //         putc(DataBuf3[i]);
        // printf("\n");
        printf("Test 4 Passed \n");
    }

    uint8_t DataBuf4[7000] ; 
    memset(DataBuf4,0,7000);  
    read_dentry_by_name((const uint8_t*)"grep",&curr);
    // printf("File found: %s Size:%u \n",curr.fileName,startINode[curr.INodeNum].bLength);
    
    if(read_data(curr.INodeNum,7000,DataBuf4,7000) != 0){
        printf("%d \n",read_data(curr.INodeNum,7000,DataBuf4,7000));
        return FAIL;
    }
    else{
        // printf("Buffer: \n");
        // for(i = 0; i<6149; i++)
        //     if (DataBuf4[i] != '\0')
        //         putc(DataBuf4[i]);
        printf("\n");
        printf("Test 5 Passed \n");
    }
    return PASS;   
}

/* File Read Test
 *
 * Tests that the file read function works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints out the file size
 * Coverage: file_read() in fs_system.c
 */
int file_read_test(){

    uint32_t fileSize =  startINode[directoryStart[10].INodeNum].bLength ;
    uint8_t buf[fileSize];
    int32_t bytes_read = file_read(directoryStart[10].INodeNum, buf, fileSize);

    printf("Bytes read: %d fileSize: %d \n" , bytes_read,fileSize);
    return PASS;
}

/* RTC Test
 *
 * Asserts that the RTC handler is being called multiple times
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: RTC functions
 */
int rtc_test(){
	TEST_HEADER;
	testing_RTC = 1; 
    if(testing_RTC) {
        clear();
        printf("Called handler \n");
        int32_t i, freq;
        int fd = open_rtc((uint8_t *)"rtc");
        int32_t list[5] = {2, 8, 32, 128, 512};
        for (i = 0; i < 5; i++) {                  // set a level of 5 different frequencies from
            write_rtc((int32_t) fd, &list[i], 4);    // the slowest to the fastest
            //printf("%u\n", w);
            for (freq = 0; freq < 20 ; freq++) {
                read_rtc((int32_t) fd, &list[i], 4);
                putc('1');
            }
            printf("\n");
        }
        // if (i = 0) {
        //     write_rtc((int32_t)fd, list, 4);
        //     for (freq = 0; freq < 10 ; freq++) {
        //         read_rtc((int32_t) fd, list, 4);
        //         putc('1');
        //     }
        // printf("\n");
        // }

        // if (i = 1) {
        //     write_rtc((int32_t)fd, list + 1, 4);
        //     for (freq = 0; freq < 10 ; freq++) {
        //         read_rtc((int32_t) fd, list + 1, 4);
        //         putc('1');
        //     }
        // printf("\n");
        // }

        // if (i = 2) {
        //     write_rtc((int32_t)fd, list + 2, 4);
        //     for (freq = 0; freq < 10 ; freq++) {
        //         read_rtc((int32_t) fd, list + 2, 4);
        //         putc('1');
        //     }
        // printf("\n");
        // }
        // if (i = 3) {
        //     write_rtc((int32_t)fd, list + 3, 4);
        //     for (freq = 0; freq < 10 ; freq++) {
        //         read_rtc((int32_t) fd, list + 3, 4);
        //         putc('1');
        //     }
        // printf("\n");
        // }
        // if (i = 4) {
        //     write_rtc((int32_t)fd, list + 4, 4);
        //     for (freq = 0; freq < 10 ; freq++) {
        //         read_rtc((int32_t) fd, list + 4, 4);
        //         putc('1');
        //     }
        // printf("\n");
        // }
    }
	return PASS; 
}


/* Terminal Read/Write Test
 *
 * Tests the terminal_read and terminal_write functions
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints out the data read by terminal_read
 * Coverage: Terminal functions
 */
int terminal_rw_test(){
    TEST_HEADER;

    char buffer[FOURKB];
    int32_t bytes;
    
    // used for testing. not needed now. uncommenting this line may cause warnings
    // set_terminal_buffer();

    while (1) {
        // test terminal read and write
        bytes = terminal_read(2, buffer, 10);
        if (bytes != 10){
            return FAIL;
        }
        terminal_write(2, buffer, bytes);

        bytes = terminal_read(2, buffer, 300);
        if (bytes > 128){
            return FAIL;
        }
        terminal_write(2, buffer, bytes);
        break;
    }
    return PASS;
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests() {
    TEST_OUTPUT("idt_test", idt_test());

    // TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("Paging test",paging_test());

    // launch your tests here
	TEST_OUTPUT("RTC test", rtc_test());
    // TEST_OUTPUT("division_by_zero_test", division_by_zero_test());
    // TEST_OUTPUT("syscall_test", syscall_test());
    // TEST_OUTPUT("paging_init_test", paging_init_test());
    // TEST_OUTPUT("paging_test", paging_test());
    // TEST_OUTPUT("kernel_up_bound_test", kernel_up_bound_test());
    // TEST_OUTPUT("kernel_low_bound_test", kernel_low_bound_test());
    // TEST_OUTPUT("vidmem_up_bound_test", vidmem_up_bound_test());
    // TEST_OUTPUT("vidmem_low_bound_test", vidmem_low_bound_test());
    // TEST_OUTPUT("null_test", null_test());
    // TEST_OUTPUT("out_of_bounds_test", out_of_bounds_test());

    /* CHECKPOINT 2 */

    // TEST_OUTPUT("Read by name test", name_search_test());
    // TEST_OUTPUT("Long file test",long_text_test());
    // TEST_OUTPUT("Read by IDX Test", idx_search_test());
    // TEST_OUTPUT("Read Directory", directory_read_test());
    // TEST_OUTPUT("Read Data Test", read_data_test_no_offset());
    // TEST_OUTPUT("Read Data Test", read_data_test_with_offset());
    // TEST_OUTPUT("File Read Test", file_read_test());
    // TEST_OUTPUT("Read by name test", name_search_test());
    //TEST_OUTPUT("Read Directory", directory_read_test());
    // TEST_OUTPUT("Read by IDX Test", idx_search_test());
    // TEST_OUTPUT("Read Data Test", read_data_test());
    // TEST_OUTPUT("File Read Test", file_read_test());
    // TEST_OUTPUT("Terminal Read/Write Test", terminal_rw_test());
}


