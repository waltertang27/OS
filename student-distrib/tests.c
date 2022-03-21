#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
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
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

int division_by_zero_test() {
    TEST_HEADER;
    int a = 0;
    int b = 1 / a;
	b = b + 1; // avoid warning
    return FAIL;
}

int syscall_test() {
    TEST_HEADER;
    __asm__("int    $0x80");
    return FAIL;
}

// tests if paging initializes
int paging_init_test(){
	TEST_HEADER;

	if(page_directory[0].present != 1 || page_directory[KERNEL_INDEX].present != 1){
		return FAIL;
	}
	// if(page_table[1].present != 1 || page_table[20].present != 1 || page_table[1000].present != 1){
	// 	return FAIL;
	// }

	return PASS;
}


// Checks that the kernel and video memory addresses can be dereferenced
int paging_test() {
	TEST_HEADER;
    
    char result;


    char* pointer = (char*)0x400000; //kernel
    result = *pointer;

    pointer = (char*)0xB8000; //video memory
    result = *pointer;

    pointer = (char*)0x7FFFFF;
    result = *pointer;
	
    pointer = (char*)0xB8FFF;
    result = *pointer;

    return PASS;
}

// Check to see if the memory before the kernel causes a page fault
int kernel_up_bound_test() {
    TEST_HEADER;
    char result;
    char* pointer = (char*)0x3FFFFF;
    result = *pointer;
    return FAIL;
}

int kernel_low_bound_test() {
    TEST_HEADER;
    char result;
    char* pointer = (char*)0x800000;
    result = *pointer;
    return FAIL;
}

int vidmem_up_bound_test() {
    TEST_HEADER;
    char result;
    char* pointer = (char*) 0xB7FFF;
    result = *pointer;
    return FAIL;
}

int vidmem_low_bound_test() {
    TEST_HEADER;
    char result;
    char* pointer = (char*) 0xB9000;
    result = *pointer;
    return FAIL;
}

int null_test() {
    TEST_HEADER;
    char result;
    char* pointer = (char*) 0;
    result = *pointer;
    return FAIL;
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	
	// TEST_OUTPUT("division_by_zero_test", division_by_zero_test());
	// TEST_OUTPUT("syscall_test", syscall_test());
	// TEST_OUTPUT("paging_init_test", paging_init_test());
	// TEST_OUTPUT("paging_test", paging_test());
	TEST_OUTPUT("kernel_up_bound_test", kernel_up_bound_test());
    // TEST_OUTPUT("kernel_low_bound_test", kernel_low_bound_test());
    // TEST_OUTPUT("vidmem_up_bound_test", vidmem_up_bound_test());
    // TEST_OUTPUT("vidmem_low_bound_test", vidmem_low_bound_test());
    // TEST_OUTPUT("null_test", null_test());
}
