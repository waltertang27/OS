#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "idt.h"
#include "rtc.h"
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


/* RTC Test
 *
 * Asserts that the RTC handler is being called multiple times
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Tests the RTC
 * Files: RTC.h RTC.c
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
        clear();
        printf("Called handler \n");
        int32_t i, freq;
        int fd = open_rtc((uint8_t *)"rtc");
        int32_t list[5] = {2, 8, 32, 128, 512};
        for (i = 0; i < 5; i++) {                  // set a level of 5 different frequencies from
            const void* j = i + list;
            write_rtc((int32_t) fd, j, 4);    // the slowest to the fastest
            for (freq = 0; freq < 10 ; freq++) {
                read_rtc((int32_t) fd, (void*)j, 4);
                putc('1');
            }
            printf("\n");
        }
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
}


