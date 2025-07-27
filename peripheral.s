.section .text

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

.globl spi_set_config
spi_set_config:
    sw a0, 0x24(tp)
    ret
