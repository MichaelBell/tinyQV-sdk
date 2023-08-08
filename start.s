.section .boot,"ax"
.globl _start
_start:
    la sp, __StackTop
    jal _bss_init
    jal main
    j .
