.macro short_isr_entry
    sw x9, -0x21c(x0)      # Save x9-x11 to scratch - note errata on WS01 & WS02 where multiple store to scratch is broken
    sw x10, -0x218(x0)
    sw x11, -0x214(x0)
.endm

.macro short_isr_exit
    lw x9, -0x21c(x0)      # Load context, x9-x11
    lw x10, -0x218(x0)
    lw x11, -0x214(x0)
    mret
.endm

.macro full_isr_entry    # Use after short_isr_entry to save all registers
    sw x12, -0x210(x0)
    sw x13, -0x20c(x0)
    sw x14, -0x208(x0)
    sw x15, -0x204(x0)
    sw x5,  -0x22c(x0)   # Save x5-x8
    sw x6,  -0x228(x0)
    sw x7,  -0x224(x0)
    sw x8,  -0x220(x0)
    mv s0, ra
    mv s1, sp
    la sp, __interrupt_stack_top
.endm

.macro isr_exit         # Exit from a full ISR
    mv ra, s0
    mv sp, s1
    lw x5, -0x22c(x0)
    lw x6, -0x228(x0)
    lw x7, -0x224(x0)
    lw x8, -0x220(x0)
    lw x9, -0x21c(x0)
    lw x10, -0x218(x0)
    lw x11, -0x214(x0)
    lw x12, -0x210(x0)
    lw x13, -0x20c(x0)
    lw x14, -0x208(x0)
    lw x15, -0x204(x0)
    mret
.endm
