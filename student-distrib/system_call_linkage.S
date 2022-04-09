#define ASM 1
#include "x86_desc.h"
.text

.globl system_call_linkage
.align 4

#linkage for system calls
system_call_linkage:
       pushl %ecx                ;\
       pushl %edx                ;\
       pushl %ebx                ;\
       pushl %esp                ;\
       pushl %ebp                ;\
       pushl %esi                ;\
       pushl %edi                ;\
       pushfl                   ;\

       pushl %edx               ;\
       pushl %ecx               ;\
       pushl %ebx               ;\

       cmpl $0, %eax        ;\
       jl invalid_cmd       ;\
       cmpl $10, %eax       ;\
       jg invalid_cmd       ;\

       call *system_call_jmp_table(, %eax, 4)  ;\

       jmp finish     ;\
        

invalid_cmd:
        movl $-1, %eax  ;\

finish:
        addl $12, %esp  ;\

        popfl         ;\

        popl %edi   ;\
        popl %esi   ;\
        popl %ebp   ;\
        popl %esp   ;\
        popl %ebx   ;\
        popl %edx   ;\
        popl %ecx   ;\
        iret        ;\
    



system_call_jmp_table:
    .long 0x00000000
    .long halt
    .long execute
    .long read
    .long write
    .long open
    .long close
    .long getargs
    .long vidmap
    .long set_handler
    .long sigreturn

