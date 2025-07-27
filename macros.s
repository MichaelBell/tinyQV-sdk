.macro isr_entry
    .2byte 0xF028      # Save context, x9-x11 to gp-0x200
.endm

.macro isr_exit
    .2byte 0x3502      # Load context, x9-x11 from gp-0x200
    mret
.endm

.macro full_isr_entry    # Use after isr_entry to save all registers
    sw4 x12, -0x1f4(gp)  # Save x12-x15 to gp-0x1f4 (following on from save context in isr_entry)
    .2byte 0xFF1C        # Save context, x1-x8 to gp-0x1E0
.endm

.macro full_isr_exit     # Use instead of isr_exit
    .2byte 0x3702        # Load context, x9-x15 from gp-0x200
    .2byte 0x37BE        # Load context, x1-x8 from gp-0x1E0
.endm
