#define ASM 1
#include "x86_desc.h"
.text

.globl system_call_linkage
.align 4

system_call_jmp_table:
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




#linkage for system calls
  #  .globl name                ;\
  #  name:                       ;\
system_call_linkage:
       pushl %ecx                ;\
       pushl %edx                ;\
       pushl %ebx                ;\
       pushl %esp                ;\
       pushl %ebp                ;\
       pushl %esi                ;\
       pushl %edi                ;\

       pushl %ebx               ;\
       pushl %ecx               ;\
       pushl %edx               ;\

       movl 4(%esp), %eax
       cmpl $1, %eax
       jl invalid_cmd
       cmpl $10, %eax
       jg invalid_cmd

       jmp *system_call_jmp_table(, %eax, 4)


        popl %edx   ;\
        popl %ecx   ;\
        popl %ebx   ;\

        popl %edi   ;\
        popl %esi   ;\
        popl %ebp   ;\
        popl %esp   ;\
        popl %ebx   ;\
        popl %edx   ;\
        popl %ecx   ;\
        iret        ;\

invalid_cmd:
    movl $-1, %eax
    popl %edx   ;\
    popl %ecx   ;\
    popl %ebx   ;\

    popl %edi   ;\
    popl %esi   ;\
    popl %ebp   ;\
    popl %esp   ;\
    popl %ebx   ;\
    popl %edx   ;\
    popl %ecx   ;\
    ret





