#include "idt.h"
#include "intr_link.h"


extern void IDT_init(void){
    unsigned int i; 
 
    for(i = 0; i< NUM_VEC; i++){
    //  currGate.offset_15_00 //idk what to do here yet
        if(i != 15 && i <EXCEP_NUM)
            idt[i].present = 1;
        else
            idt[i].present = 0; 
        
        idt[i].seg_selector = KERNEL_CS ;
        idt[i].reserved0 = 0;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 0;
        idt[i].size = 1; 
        idt[i].dpl = 0;
    }
    
    SET_IDT_ENTRY(idt[0],div_zero_excep);
    SET_IDT_ENTRY(idt[1],debug_excep);
    SET_IDT_ENTRY(idt[2],nmi_excep);
    SET_IDT_ENTRY(idt[3],breakpoint_excep);
    SET_IDT_ENTRY(idt[4],overflow_excep);
    SET_IDT_ENTRY(idt[5],bound_excep);
    SET_IDT_ENTRY(idt[6],invalid_opcode_excep);
    SET_IDT_ENTRY(idt[7],device_not_avail_excep);
    SET_IDT_ENTRY(idt[8],double_fault_excep);
    SET_IDT_ENTRY(idt[9],coprocessor_excep);
    SET_IDT_ENTRY(idt[10],invalid_tss_excep);
    SET_IDT_ENTRY(idt[11],segment_not_present_excep);
    SET_IDT_ENTRY(idt[12],stack_fault_excep);
    SET_IDT_ENTRY(idt[13],gen_protec_excep);
    SET_IDT_ENTRY(idt[14],page_fault_excep);
    SET_IDT_ENTRY(idt[15],fpu_excep);
    SET_IDT_ENTRY(idt[16],allignment_excep);
    SET_IDT_ENTRY(idt[17],machine_check_excep);
    SET_IDT_ENTRY(idt[18],simd_excep);

    
    
    idt[33].present = 1;
    idt[33].reserved3 = 1;
    SET_IDT_ENTRY(idt[33], keyboard_handler_linkage);
    
    /*
    idt[40].present = 1;
    idt[40].reserved3 = 1;
    SET_IDT_ENTRY(idt[40], rtc_handler_linkage);
*/
    lidt(idt_desc_ptr);
}


void blue_screen(char * exp_name){
    printf("Exception: %s \n",exp_name);
    while (1){}   ; 
}


//Individual exception handlers
extern void div_zero_excep(){
    char exception[] = "Divide by zero" ; 
    blue_screen(exception);
}

extern void debug_excep(){
    char exception[] = "Debug" ; 
    blue_screen(exception);
}
extern void nmi_excep(){
    char exception[] = "NMI" ; 
    blue_screen(exception);
}
extern void breakpoint_excep(){
     char exception[] = "breakpoint" ; 
    blue_screen(exception);
}
extern void overflow_excep(){
     char exception[] = "overflow" ; 
    blue_screen(exception);
}
extern void bound_excep(){
     char exception[] = "bound" ; 
    blue_screen(exception);
}
extern void invalid_opcode_excep(){
     char exception[] = "invalid opcode" ; 
    blue_screen(exception);
}
extern void device_not_avail_excep(){
     char exception[] = "device not available" ; 
    blue_screen(exception);
}
extern void double_fault_excep(){
     char exception[] = "double fault" ; 
    blue_screen(exception);
}
extern void coprocessor_excep(){
    char exception[] = "coprocessor overrun" ; 
    blue_screen(exception);
}
extern void invalid_tss_excep(){
    char exception[] = "invalid tss" ; 
    blue_screen(exception);
}
extern void segment_not_present_excep(){
    char exception[] = "segment not present" ; 
    blue_screen(exception);
}
extern void stack_fault_excep(){
    char exception[] = "stack fault" ; 
    blue_screen(exception);
}
extern void gen_protec_excep(){
    char exception[] = "general protection" ; 
    blue_screen(exception);
}
extern void page_fault_excep(){
    char exception[] = "page fault" ; 
    blue_screen(exception);
}
extern void fpu_excep(){
    char exception[] = "fpu" ; 
    blue_screen(exception);
}
extern void allignment_excep(){
    char exception[] = "allignment" ; 
    blue_screen(exception);
}
extern void machine_check_excep(){
    char exception[] = "machine check" ; 
    blue_screen(exception);
}
extern void simd_excep(){
    char exception[] = "simd" ; 
    blue_screen(exception);
}

