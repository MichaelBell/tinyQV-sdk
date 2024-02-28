.macro isr_entry
    .2byte 0xF024      # Save context, x9-x10 to gp-0x200
.endm

.macro isr_exit
    .2byte 0x3482      # Load context, x9-x10 from gp-0x200
    mret
.endm
