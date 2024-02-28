.macro trap_entry
    .2byte 0xF028      # Save context, x9-x11 to gp-0x200
.endm

.macro trap_exit
    .2byte 0x3502      # Load context, x9-x11 from gp-0x200
.endm

.section .boot,"ax"
_boot:
    j _start          # Reset
    j _trap_handler   # Trap
    trap_entry        # Interrupt, vectored here
    csrr a0, mcause   # This _boot sequence in total must be less than 0x40 bytes
    andi s1, a0, 0x1F
    slli s1, s1, 2
    lw s1, (s1)
    jalr s1, (s1)
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

# These ISRs are entered with only a1, a2 (and a3, containing the return address) are saved
# The ISR must save and restore all other registers it modifies.
# a0 is set to mcause on entry, so 1 << a0 is the corresponding bit in mip/mie
.globl isr_in0, isr_in1, isr_uart_byte_available, isr_uart_writable
.weak isr_in0, isr_in1, isr_uart_byte_available, isr_uart_writable
isr_in0:  # Default implementation just acks the interrupt
isr_in1:
    li a1, 1
    sll a1, a1, a0
    csrc mip, a1
    jalr x0, (s1)

isr_uart_byte_available:  # Default implementation clears interrupt enable 
isr_uart_writable:        # to avoid an infinite interrupt loop
    li a1, 1
    sll a1, a1, a0
    csrc mie, a1
    jalr x0, (s1)
