.section .text
.globl uart_putc
uart_putc:
    lw a5, 0x14(tp)
    andi a5, a5, 1
    bnez a5, uart_putc
    sw a0, 0x10(tp)
    ret

.globl uart_getc
uart_getc:
    li a0, -1
    lw a5, 0x14(tp)
    andi a5, a5, 2
    beqz a5, 1f
    lw a0, 0x10(tp)
1:
    ret

.globl uart_is_char_available
uart_is_char_available:
    lw a0, 0x14(tp)
    srli a0, a0, 1
    andi a0, a0, 1
    ret
