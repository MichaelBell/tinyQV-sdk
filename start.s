.include "macros.s"

.section .boot,"ax"
_boot:
.option norvc
    j _start          # Reset
    j _trap_handler   # Trap
.option rvc    
    short_isr_entry   # Interrupt, vectored here
    csrr a0, mcause
    li a1, 7
    beq a0, a1, _isr_timer
    addi a1, a0, 0x20
    slli a1, a1, 2
    lw a1, (a1)
    andi s1, a0, 0xC
    bnez s1, _isr_user
    jr a1

_start:
    li gp, 0x1000400    # These required if testing against
    li tp, 0x8000000    # a different Risc-V core, but do no harm on TinyQV
    la sp, __StackTop
    jal __runtime_init
    li a1, 0x40000      # Enable UART receive interupt
    csrs mie, a1
    call main
    j .

# The trap handler should probably do something more useful.
_trap_handler:
    mret

_isr_timer:
    full_isr_entry
    call tqv_timer_interrupt
    isr_exit

_isr_user:
    full_isr_entry
    jalr ra, (a1)
    isr_exit

.section .vectors,"a"
    .word isr_in0    # 0xc0-0xfc is vectors for the custom interrupts
    .word isr_in1
    .word isr_uart_byte_available
    .word isr_uart_writable
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt
    .word tqv_default_user_interrupt

.section .text

# These ISRs are entered with only s1, a0, a1 saved
# The ISR must save and restore any other registers it modifies.
# a0 is set to mcause on entry, so 1 << a0 is the corresponding bit in mip/mie
.globl isr_in0, isr_in1, isr_uart_byte_available, isr_uart_writable, isr_user_unused, tqv_timer_interrupt
.weak isr_in0, isr_in1, isr_uart_byte_available, isr_uart_writable, tqv_timer_interrupt
isr_in0:  # Default implementation just acks the interrupt
isr_in1:
    li a1, 1
    sll a1, a1, a0
    csrc mip, a1
    short_isr_exit

isr_uart_byte_available:  # Default implementation clears interrupt enable 
isr_uart_writable:        # to avoid an infinite interrupt loop
    li a1, 1
    sll a1, a1, a0
    csrc mie, a1
    short_isr_exit

# This is designed to be implemented in C, it's a normal function.
tqv_timer_interrupt:      # Default implementation just clears interrupt enable
    li a0, 0x80
    csrc mie, a0
    ret

# A user interrupt - again this is a normal function that can be implemented in C.
# Default implementation just disables the interrupt and returns
# a0 contains mcause.
.globl tqv_default_user_interrupt
tqv_default_user_interrupt:
    li a1, 1
    sll a1, a1, a0
    csrc mie, a1
    ret
