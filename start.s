.macro trap_entry
    sw ra, -0x400(gp)
    sw a0, -0x3fc(gp)
    sw a1, -0x3f8(gp)
.endm

.macro trap_exit
    lw a1, -0x3f8(gp)
    lw a0, -0x3fc(gp)
    lw ra, -0x400(gp)
.endm

.section .boot,"ax"
_boot:
    j _start          # Reset
    j _trap_handler   # Trap
    trap_entry        # Interrupt, vectored here
    csrr a0, mcause   # This _boot sequence in total must be 0x34 bytes
    andi x1, a0, 0x1F
    slli x1, x1, 2
    lw x1, (x1)
    jalr x1
    trap_exit
    mret

.section .vectors,"a"
    .word isr_in0    # 0x40-0x4c is vectors for the custom interrupts
    .word isr_in1
    .word isr_uart_byte_available
    .word isr_uart_writable

.section .text
.globl _start
_start:
    li gp, 0x1000400    # These required if testing against
    li tp, 0x8000000    # a different Risc-V core, but do no harm on TinyQV
    la sp, __StackTop
    jal __runtime_init
    jal main
    j .

# The trap
.globl _trap_handler
_trap_handler:
    mret

# These ISRs are entered with only a0, a1 (and ra) saved
# The ISR must save and restore all other registers it modifies.
# a0 is set to mcause on entry, so 1 << a0 is the corresponding bit in mip/mie
.globl isr_in0, isr_in1, isr_uart_byte_available, isr_uart_writable
.weak isr_in0, isr_in1, isr_uart_byte_available, isr_uart_writable
isr_in0:  # Default implementation just acks the interrupt
isr_in1:
    li a1, 1
    sll a1, a1, a0
    csrc mip, a1
    ret

isr_uart_byte_available:  # Default implementation clears interrupt enable 
isr_uart_writable:        # to avoid an infinite interrupt loop
    li a1, 1
    sll a1, a1, a0
    csrc mie, a1
    ret
