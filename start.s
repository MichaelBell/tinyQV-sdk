.include "macros.s"

.section .boot,"ax"
_boot:
.option norvc
    j _start          # Reset
    j _trap_handler   # Trap
.option rvc    
    short_isr_entry   # Interrupt, vectored here
    csrr a0, mcause
    slli s1, a0, 2
    li a1, 28
    beq s1, a1, _isr_timer
    addi a1, s1, 0x80
    lw a1, (a1)
    andi s1, s1, 0x30
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
    .word tqv_user_interrupt04
    .word tqv_user_interrupt05
    .word tqv_user_interrupt06
    .word tqv_user_interrupt07
    .word tqv_user_interrupt08
    .word tqv_user_interrupt09
    .word tqv_user_interrupt10
    .word tqv_user_interrupt11
    .word tqv_user_interrupt12
    .word tqv_user_interrupt13
    .word tqv_user_interrupt14
    .word tqv_user_interrupt15

.section .text

# These ISRs are entered with only s1, a0, a1 saved
# The ISR must save and restore any other registers it modifies.
# a0 is set to mcause on entry, so 1 << a0 is the corresponding bit in mip/mie
.globl isr_in0, isr_in1, isr_uart_byte_available, isr_uart_writable, tqv_timer_interrupt
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

# Interrupt redirects, this allows a specific interrupt function to be
# installed simply by defining a function tqv_user_interruptnn
.globl tqv_user_interrupt04, tqv_user_interrupt05, tqv_user_interrupt06, tqv_user_interrupt07
.globl tqv_user_interrupt08, tqv_user_interrupt09, tqv_user_interrupt10, tqv_user_interrupt11
.globl tqv_user_interrupt12, tqv_user_interrupt13, tqv_user_interrupt14, tqv_user_interrupt15
.weak tqv_user_interrupt04, tqv_user_interrupt05, tqv_user_interrupt06, tqv_user_interrupt07
.weak tqv_user_interrupt08, tqv_user_interrupt09, tqv_user_interrupt10, tqv_user_interrupt11
.weak tqv_user_interrupt12, tqv_user_interrupt13, tqv_user_interrupt14, tqv_user_interrupt15
tqv_user_interrupt04:
tqv_user_interrupt05:
tqv_user_interrupt06:
tqv_user_interrupt07:
tqv_user_interrupt08:
tqv_user_interrupt09:
tqv_user_interrupt10:
tqv_user_interrupt11:
tqv_user_interrupt12:
tqv_user_interrupt13:
tqv_user_interrupt14:
tqv_user_interrupt15:
    j tqv_user_interrupt

# A user interrupt - again this is a normal function that can be implemented in C.
# Default implementation just disables the interrupt and returns
# a0 contains mcause.
.globl tqv_user_interrupt
.weak tqv_user_interrupt
tqv_user_interrupt:
    li a1, 1
    sll a1, a1, a0
    csrc mie, a1
    ret
