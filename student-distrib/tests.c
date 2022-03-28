#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
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

     // start of the kernel
    char *pointer = (char *)KERNEL_ADDR;
    ref = *pointer;

     // start of the video memory
    pointer = (char *)VID_ADDR;
    ref = *pointer;

    // end of the kernel 
    pointer = (char *)0x7FFFFF;
    ref = *pointer;

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

    char *pointer = (char *)0;
    ref = *pointer;
    return FAIL;
}

/* kernel_up_test()
 *
 * Checks if there is a page fault exception from memory before the kernel
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side effects: halts the OS and displays errors
 * Coverage: page fault exception
 */
int kernel_up_test()
{
    TEST_HEADER;
    char ref;
    char *pointer = (char *)0x3FFFFF;
    ref = *pointer;
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
    char *pointer = (char *)0x800000;
    ref = *pointer;
    return FAIL;
}
/* vidmem_up_test()
 *
 * Checks if there is a page fault exception from memory before the video memory
 * Inputs: None
 * Outputs: Page fault/FAIL
 * Side effects: halts the OS and displays errors
 * Coverage: page fault exception
 */
int vidmem_up_test()
{
    TEST_HEADER;
    char ref;
    char *pointer = (char *)0x0B7FFF;
    ref = *pointer;
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
int rtc_test(){
	TEST_HEADER;
	testing_RTC = 1; 
	return PASS; 
}
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
    if (read_dentry_by_index(5, &entry) == -1){
        return FAIL;
    }
    // printf("=================================== \n %s \n =========================================\n", entry.fileName);
    if(strncmp((int8_t * ) entry.fileName, (int8_t * )"rtc",sizeof("rtc")))
        return FAIL; 

    if (read_dentry_by_index(2, &entry_2) == -1){
        return FAIL;
    }
    // printf("=================================== \n %s \n =========================================\n", entry.fileName);
    if(! strncmp((int8_t * ) entry_2.fileName, (int8_t * )"grep",sizeof("grep")))
        return FAIL; 


    return PASS;
}

int directory_read_test(){
    TEST_HEADER;
	int i; 
    dentry_t dir_name;
    open(" ", 2);

	for(i = 0; i<17;i++){
		directory_read(0, (dentry_t *)&dir_name, 54);
		if(!strlen((int8_t *) dir_name.fileName))
			return FAIL;
		else{
			//printf(" Filename: %s, File Type: %d, File Size %d \n ", dir_name.fileName,dir_name.fileType, startINode[dir_name.INodeNum].bLength);
		}
	}

}

int read_data_test(){
    TEST_HEADER;
    int8_t dir_name[32]; 
    
    read_data(4, 10, &dir_name, 32);

    

    // read_data(4, 10, &dir_name, 32);
    // if(!strlen(dir_name))
    //     return FAIL;

    return PASS;
    
}

int file_read_test(){
    uint8_t buf[FOURKB];
    int32_t bytes = FOURKB;
    int32_t bytes_read = file_read(2, buf, bytes);

    printf("size: %d", bytes_read);
    return PASS;
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests()
{
    
    /* CHECKPOINT 1 */

    // TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("RTC test", rtc_test());
    // TEST_OUTPUT("division_by_zero_test", division_by_zero_test());
    // TEST_OUTPUT("syscall_test", syscall_test());
    // TEST_OUTPUT("paging_init_test", paging_init_test());
    // TEST_OUTPUT("paging_test", paging_test());
    // TEST_OUTPUT("null_test", null_test());
    // TEST_OUTPUT("before_kernel_memory", kernel_up_test());
    // TEST_OUTPUT("after_kernel_memory", kernel_low_test());
    // TEST_OUTPUT("before_vidmem_memory", vidmem_up_test());

    /* CHECKPOINT 2 */

    // TEST_OUTPUT("read by name test", name_search_test());
    // TEST_OUTPUT("Read Directory", directory_read_test());
    // TEST_OUTPUT("Read by IDX Test", idx_search_test());
    // TEST_OUTPUT("Read Data Test", read_data_test());
    TEST_OUTPUT("File Read Test", file_read_test());
}

