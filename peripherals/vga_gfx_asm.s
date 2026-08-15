# VGA graphics (Asteroids peripheral 9) asm functions

.section .text.vga_gfx

.extern vga_gfx_front_buffer

.globl tqv_user_interrupt09_raw
tqv_user_interrupt09_raw:
    sw2 x12, -0x1f4(gp)  # Save x12-x13 to gp-0x1f4 (following on from save context in isr_entry)
    lbu a3, 0x243(tp)
    addi a0, a0, 23  # Assumes a0 contains 25 (because this is interrupt 9), to make 48 with a compressed instr
    mul16 a3, a3, a0
    lbu a1, 0x242(tp)
    add a1, a1, a3
    addi a1, a1, 1
    slti a0, a1, 768
    li a3, 1020
    mul16 a0, a0, a3
    and a1, a1, a0
    slli a1, a1, 0x4

    lbu x0, 0x241(tp)

    la s1, vga_gfx_front_buffer
    lw s1, (s1)
    add s1, s1, a1

    lw4 a0, (s1)
    sw4 a0, 0x240(tp)
    lw4 a0, 0x10(s1)
    sw4 a0, 0x250(tp)
    lw4 a0, 0x20(s1)
    sw4 a0, 0x260(tp)
    lw4 a0, 0x30(s1)
    sw4 a0, 0x270(tp)
    
    .2byte 0x3602       # Load context, x9-x13 from gp-0x200
    mret

# Implement in this text section to force
# interrupt handler to be added if vga_gfx is started
.globl vga_gfx_start
vga_gfx_start:
    li a5, 0         # Ensure debug off
    sw a5, 0x30(tp)
    li a5, 0xff
    sw a5, 0xc(tp)

    lui a5, 0x8000   # All outputs to peripheral 9
    addi a4, a5, 0x80
    addi a5, a5, 0x60
    li a3, 9
1:
    sw a3,0(a5)
    addi a5, a5, 4
    blt a5, a4, 1b

    li a5, 0x0c     # Interrupt config
    sw a5, 0x241(tp)

    lui a5, 0x2000  # Enable interrupt 9
    csrs mie, a5
    ret

.globl vga_gfx_clear
vga_gfx_clear:
    li a2, 0x55555555
    mul16 a2, a2, a0
    li a3, 12288         # Length of buffer
    add a3, a3, a1

1:
    sw4n a2, (a1)
    sw4n a2, 0x10(a1)
    sw4n a2, 0x20(a1)
    sw4n a2, 0x30(a1)
    addi a1, a1, 0x40
    blt a1, a3, 1b

    ret
