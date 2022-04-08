#define ASM 1

.globl flush_tlb

.align 4

flush_tlb:
    movl %cr3, %eax    
    movl %eax, %cr3
    ret
