.macro short_isr_entry
    sw2 x10, -0x318(x0)     # Save x9-x11 to scratch
    sw x9, -0x31c(x0)      
.endm

.macro short_isr_exit
    lw2 x10, -0x318(x0)     # Load context, x9-x11
    lw x9, -0x31c(x0)      
    mret
.endm

.macro full_isr_entry    # Use after short_isr_entry to save all registers
    sw4 x12, -0x310(x0)
    sw4 x5,  -0x330(x0)   # Save x5-x8
    mv s0, ra
    mv s1, sp
    la sp, __interrupt_stack_top
.endm

.macro isr_exit         # Exit from a full ISR
    mv ra, s0
    mv sp, s1
    lw4 x5, -0x330(x0)
    lw x9, -0x31c(x0)
    lw2 x10, -0x318(x0)
    lw4 x12, -0x310(x0)
    mret
.endm
