#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
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
    
}

// add more tests here


void launch_tests()
{

    TEST_OUTPUT("idt_test", idt_test());

    // TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("Paging test",paging_test());

    // launch your tests here
	// TEST_OUTPUT("RTC test", rtc_test());
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

