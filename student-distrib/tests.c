#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "idt.h"
#include "rtc.h"
#include "fs_system.h"
#include "terminal.h"
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
    int num1, num2;


    num1 = 0;
    num2 = 1 / num1;
    num2 = num2 + 1; // avoid warning
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

    if (page_directory[0].present != 1){
        return FAIL;
    }

    if (page_directory[1].present != 1){
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

     // start of the kernel
    char * temp = (char * )KERNEL_ADDR;
    ref = *  temp;

     // start of the video memory
    temp = (char *  )VID_ADDR;
    ref = * temp;

    // middle of the kernel 
    temp = (char * )0x6BF312;
    ref = * temp;

    // end of the kernel 
    temp = (char * )0x7FFFFF;
    ref = * temp;

    return PASS;
}

/* null_test()
*
* Checks if deferencing NULL gives page fault exception
* Inputs: None
* Outputs: Page fault/FAIL
* Side effects: Halts the OS and displays errors
* Coverage: page fault exception
 */
int null_test()
{
    TEST_HEADER;

    char ref;

    // convert 0 to char pointer for NULL

    char * temp = (char * ) 0x0;
    ref = * temp;
    return FAIL;
}

/* kernel_high_test()
 *
 * Checks if there is a page fault exception from memory before the kernel
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side effects: halts the OS and displays errors
 * Coverage: page fault exception
 */
int kernel_high_test()
{
    TEST_HEADER;

    char ref;

    // char * temp = (char * ) 0x3FFFFF;
    // ref = * temp;
    char * temp = (char * ) 0x3FF998;
    ref = * temp;
    return FAIL;
}

/* kernel_low_test()
 *
 * Checks if there is a page fault exception from memory after the kernel
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side effects: halts the OS and displays errors
 * Coverage: page fault exception
 */
int kernel_low_test()
{
    TEST_HEADER;

    char ref;

    // char * temp = (char * ) 0x800000;
    // ref = * temp;

    char * temp = (char * ) 0x800365;
    ref = * temp;
    return FAIL;
}
/* video_mem_test()
 *
 * Checks if there is a page fault exception from memory outside of the video memory
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side effects: halts the OS and displays errors
 * Coverage: page fault exception
 */
int video_mem_test()
{
    TEST_HEADER;

    char ref;

    char * temp = (char * ) 0x0B7ABC;
    ref = * temp;
    return FAIL;
}

/* RTC Test
 *
 * Asserts that the RTC handler is being called multiple times
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: RTC functions
 */
/* Checkpoint 2 tests */

int name_search_test(){
    TEST_HEADER;
    dentry_t entry; 
    uint8_t * word = (uint8_t * )"cat";
    if (read_dentry_by_name(word, &entry) == -1){
        return FAIL;
    }

    word = (uint8_t * )"FFFF";
    if (read_dentry_by_name(word, &entry) == -1){
        return PASS;
    }
    
    return FAIL;
}

int idx_search_test(){
    TEST_HEADER;
    dentry_t entry, entry_2; 
    //int i; 
    if (read_dentry_by_index(5, &entry) == -1){
        return FAIL;
    }

    if(strncmp((int8_t * ) entry.fileName, (int8_t * )"rtc",sizeof("rtc")))
        return FAIL; 

    if (read_dentry_by_index(2, &entry_2) == -1){
        return FAIL;
    }
    if(! strncmp((int8_t * ) entry_2.fileName, (int8_t * )"grep",sizeof("grep")))
        return FAIL; 
    // for(i =0; i<62; i = i+3){
    //     printf("Inode:%u Bytes:%u Inode:%u Bytes:%u Inode:%u Bytes:%u \n",i,startINode[i].bLength,i+1,startINode[i].bLength,i+2,startINode[i+2].bLength);
    // }
    return PASS;
}

int directory_read_test(){
    TEST_HEADER;
	int i; 
    dentry_t dir_name;
    open((uint8_t *)" ", 2);

	for(i = 0; i<17;i++){
		directory_read(0, (dentry_t *)&dir_name, 54);
		if(!strlen((int8_t *) dir_name.fileName))
			return FAIL;
		else{
		    printf(" Filename: %s, File Type: %d, File Size %d \n ", dir_name.fileName,dir_name.fileType, startINode[dir_name.INodeNum].bLength);
		}
	}
    return PASS; 
}

int read_data_test(){
    TEST_HEADER;
    uint8_t DataBuf[300] ; 
    memset(DataBuf,0,300);

    dentry_t curr; 
    read_dentry_by_index(10,&curr);

    
    read_data(curr.INodeNum, 0, DataBuf, 300);
    // if(!strlen(dir_name))
    //     return FAIL;

    printf(" Buffer: %s \n",DataBuf);
    return PASS;
    
}

int file_read_test(){
    uint8_t buf[FOURKB];
    int32_t bytes = FOURKB;
    int32_t bytes_read = file_read(2, buf, bytes);



    printf("size: %d", bytes_read);
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
// int rtc_test(){
// 	TEST_HEADER;
// 	testing_RTC = 1; 
// 	return PASS; 
// }

/* Checkpoint 2 tests */
int rtc_test(){
	TEST_HEADER;
	testing_RTC = 1;
    if(testing_RTC) {
        printf("Called handler \n");
        int i, j, freq;
        int fd = open_rtc((uint8_t *)"rtc");
        int32_t list[5] = {2, 8, 32, 128, 512};
        for (i = 0; i < 5; i++) {                  // set a level of 5 different frequencies from
            j = (int32_t)i + (int32_t)list;
            write_rtc((int32_t) fd, (const void *)j, (int32_t)4);                   // the slowest to the fastest
            // printf("Freqency: %d Hz \n", list[i]);
            for (freq = 0; freq < list[i] ; freq++) {
                read_rtc((int32_t) fd, (void*)freq,(int32_t) 4);
                putc('1');
            }
            printf("\n");
        }
        // close_rtc(fd);
    }
	return PASS; 
}


/* RTC Test
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
    clear(); 
    //TEST_OUTPUT("idt_test", idt_test());

    // TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("Paging test",paging_test());

    // launch your tests here
	TEST_OUTPUT("RTC test", rtc_test());
    // TEST_OUTPUT("division_by_zero_test", division_by_zero_test());
    // TEST_OUTPUT("paging_init_test", paging_init_test());
    // TEST_OUTPUT("paging_test", paging_test());
    // TEST_OUTPUT("null_test", null_test());
    // TEST_OUTPUT("before_kernel_memory", kernel_high_test());
    // TEST_OUTPUT("after_kernel_memory", kernel_low_test());
    // TEST_OUTPUT("video memory test", video_mem_test());

    /* CHECKPOINT 2 */

    // TEST_OUTPUT("read by name test", name_search_test());
    // TEST_OUTPUT("Read Directory", directory_read_test());
    // TEST_OUTPUT("Read by IDX Test", idx_search_test());
    // TEST_OUTPUT("Read Data Test", read_data_test());
    // TEST_OUTPUT("File Read Test", file_read_test());

    //TEST_OUTPUT("Terminal Read/Write Test", terminal_rw_test());
}


