#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"

// typedef union idt_desc_t {
//     uint32_t val[2];
//     struct {
//         uint16_t offset_15_00;
//         uint16_t seg_selector;
//         uint8_t  reserved4;
//         uint32_t reserved3 : 1;
//         uint32_t reserved2 : 1;
//         uint32_t reserved1 : 1;
//         uint32_t size      : 1;
//         uint32_t reserved0 : 1;
//         uint32_t dpl       : 2;
//         uint32_t present   : 1;
//         uint16_t offset_31_16;
//     } __attribute__ ((packed));
// } idt_desc_t;

extern void IDT_init(void){
    // Do all the stuff 
}


void blue_screen(char * exp_name){
    printf(exp_name);
    while (1){}    
}

extern void handleException(int num){
    char *exception; 
    switch (num)
    {
    case 0:
        exception = "Divide by 0 exception " ;
        break;
    case 1: 
        exception = "Debug Exception " ;
        break;
    case 2: 
        exception = "NMI Interrupt " ;
        break;
    case 3: 
        exception = "Breakpoint Exception " ;
        break;
    case 4: 
        exception = "Overflow Exception " ;
        break;
    case 5: 
        exception = "BOUND Range Exceeded Exception " ;
        break;
    case 6: 
        exception = "Invalid Opcode Exception " ;
        break;
    case 7: 
        exception  = "Device not Available Exception " ;
        break;
    case 8: 
        exception  = "Double Fault Exception " ;
        break;
    case 9: 
        exception  = "Copressor Segment Overrun " ;
        break;
    case 10: 
        exception  = "Invalid TSS Exception " ;
        break;
    case 11: 
        exception  = "Segment not Present " ;
        break;
    case 12: 
        exception  = "Stack Fault Exception " ;
        break;
    case 13: 
        exception  = "General Protection Exception " ;
        break;
    case 14: 
        exception  = "Page fault exception " ;
        break;
    case 15: 
        exception  = "I think its second pic lol idk yet " ;
        break;
    case 16: 
        exception  = "x87 FPU Floating-Point Error " ;
        break;
    case 17: 
        exception  = "Allignment Check Exception " ;
        break;
    case 18: 
        exception  = "Machine-Check Exception " ;
        break;
    case 19: 
        exception  = "SIMD Floating-Point Exception " ;
        break;
    default:
        exception  = "Out of interrupt range" ;
        break;
    }
    blue_screen(exception);
}

