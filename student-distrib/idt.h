#ifndef IDT_H_
#define IDT_H_


#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "intr_link.h"
#include "tests.h"
//#include "keyboard.h"
#include "intr_link.h"
#include "keyboard.h"
#include "systemcalls.h"
#define EXCEP_NUM 19
#define RTC_INTERRUPT 40 
#define KEYBOARD_IDT    33
#define SYS_IDT 0x80




extern void IDT_init(void);
void blue_screen(char * exception);

extern void div_zero_excep();
extern void debug_excep();
extern void nmi_excep();
extern void breakpoint_excep();
extern void overflow_excep();
extern void bound_excep();
extern void invalid_opcode_excep();
extern void device_not_avail_excep();
extern void double_fault_excep();
extern void coprocessor_excep();
extern void invalid_tss_excep();
extern void segment_not_present_excep();
extern void stack_fault_excep();
extern void gen_protec_excep();
extern void page_fault_excep();
extern void fpu_excep();
extern void allignment_excep();
extern void machine_check_excep();
extern void simd_excep();
extern void system_call_linkage(); 
extern void test_system(); 


extern int32_t idt_flag; 


#endif

