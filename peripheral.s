.section .text

.globl uart_getc
uart_getc:
    li a0, -1
    #lw a5, 0x14(tp)
    .insn ci 2, 3, a5, 0x14
    andi a5, a5, 2
    beqz a5, 1f
    #lw a0, 0x10(tp)
    .insn ci 2, 3, a0, 0x10
1:
    ret

.globl uart_is_char_available
uart_is_char_available:
    #lw a0, 0x14(tp)
    .insn ci 2, 3, a0, 0x14
    srli a0, a0, 1
    andi a0, a0, 1
    ret

.globl debug_uart_putc
debug_uart_putc:
    .insn ci 2, 3, a5, 0x1c
    andi a5, a5, 1
    bnez a5, debug_uart_putc
    .insn css 2, 7, a0, 0x18
    ret
