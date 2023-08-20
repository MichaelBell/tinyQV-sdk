.section .boot,"ax"
.globl _start
_start:
    li gp, 0x1000
    li tp, 0x10000000
    la sp, __StackTop
    jal _bss_init
    jal main
    j .
