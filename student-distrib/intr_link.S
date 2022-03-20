#define ASM 


#define INTR_LINK(name, func)   \
    .globl name                ;\
    name:                       ;\
        pushal                  ;\
        pushfl                  ;\
        call func               ;\
        popfl                   ;\
        popal                   ;\

#INTR_LINK(rtc_handler_linkage, rtc_handler);
INTR_LINK(keyboard_handler_linkage, keyboard_handler);

