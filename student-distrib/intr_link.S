#define ASM 

#linkage for rtc and keyboard rtc_handler
#INTR_LINK function takes in the linkage function and the handler so that they are compatiable (name, func)
#define INTR_LINK(name, func)   \
    .globl name                ;\
    name:                       ;\
        pushal                  ;\
        pushfl                  ;\
        call func               ;\
        popfl                   ;\
        popal                   ;\
        iret                    ;\

INTR_LINK(keyboard_handler_linkage, keyboard_handler);
INTR_LINK(rtc_handler_linkage, rtc_handler);
