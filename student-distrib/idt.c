#include "idt.h"
#include "intr_link.h"

/*
DESCRIPTION: initializes the IDT with the first 18 entries
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: IDT can be called with coresponding handlers
*/
extern void IDT_init(void){
    unsigned int i; 
 
    for(i = 0; i< NUM_VEC; i++){
    // If the value is not 15(reserved) and under 19 then make it present 
        if(i != 15 && i <EXCEP_NUM)
            idt[i].present = 1;
        else
            idt[i].present = 0; 
        
        //Make the segment for all interrupt the CS Kernel 
        idt[i].seg_selector = KERNEL_CS ;

        // 16 bit interrupt gates have reserved bits 0110 
        idt[i].reserved0 = 0;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 0;

        // DPL is 0 to prevent user level programs from acessing interrupts 
        idt[i].size = 1; 
        // Standard size is 1
        idt[i].dpl = 0;
    }
    
    // Each value in the IDT array refrences the exception defined by intel 0-18
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

    
    
    idt[KEYBOARD_IDT].present = 1;
    idt[KEYBOARD_IDT].reserved3 = 1;
    SET_IDT_ENTRY(idt[KEYBOARD_IDT],keyboard_handler_linkage);
    

    idt[RTC_INTERRUPT].present = 1;                            
    SET_IDT_ENTRY(idt[RTC_INTERRUPT], rtc_handler_linkage);

    SET_IDT_ENTRY(idt[SYS_IDT], )

    idt[SYS_IDT].present = 1;
    SET_IDT_ENTRY(idt[SYS_IDT], system_call_linkage);
    idt[SYS_IDT].dpl = 0x3;         //set dpl level to level 3 forr correct privilege

    lidt(idt_desc_ptr);
}

/*
DESCRIPTION: Stops the operating system when an exception is called 
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS:Kernel loops forever 
*/
void blue_screen(char * exp_name){
    printf("Exception: %s \n",exp_name);
    while (1){}   ; 
}


/*
DESCRIPTION: Prints corresponding exception handler and called blue screen 
INPUTS: none
OUTPUTS: none
RETURN VALUE: none
SIDE EFFECTS: Prints to screen and halts OS 
*/

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

