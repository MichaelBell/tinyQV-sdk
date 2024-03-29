.section .text

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

.globl debug_uart_putc
debug_uart_putc:
    lw a5, 0x1c(tp)
    andi a5, a5, 1
    bnez a5, debug_uart_putc
    sw a0, 0x18(tp)
    ret

.globl spi_send_data
spi_send_data:
    lw a5, 0x24(tp)
    andi a5, a5, 1
    bnez a5, spi_send_data
    sw a0, 0x20(tp)
    ret

.globl spi_send_recv_data
spi_send_recv_data:
    lw a5, 0x24(tp)
    andi a5, a5, 1
    bnez a5, spi_send_recv_data
    sw a0, 0x20(tp)
1:
    lw a5, 0x24(tp)
    andi a5, a5, 1
    bnez a5, 1b
    lw a0, 0x20(tp)
    ret

.globl spi_set_divider
spi_set_divider:
    sw a0, 0x24(tp)
    ret
