.section .text

.globl uart_getc
uart_getc:
    li a0, -1
    lw a5, 0x14(tp)
    andi a5, a5, 2
    beqz a5, 1f
    lw a0, 0x10(tp)

/*  We should now just return.  But sadly on the TT06 tinyQV, UART RX always reads 0.
    So the plan to workaround this is to make the RP2040 repeat every character.
    When the character is read from the UART device that will unblock CTS so the RP2040
    should immediately send the repeat.  We will then watch the UART receive pin for the 
    repeated character and report it. */

    li a3, 8
    li a5, 0x80

    /* Wait for in7 low (start condition) */
2:
    lw a0, 0x4(tp)
    and a0, a0, a5
    bnez a0, 2b

    /* Wait 1 bit clock for each bit */
    rdcycle a2
4:
    srli a0, a0, 1
    addi a2, a2, 69  /* ~1 bit clock (64MHz = 69) */
3:
    rdcycle a1
    bltu a1, a2, 3b
    lw a4, 0x4(tp)
    and a4, a4, a5
    add a0, a0, a4
    addi a3, a3, -1
    bnez a3, 4b

    /* Now we need to wait for the character to be available from the UART block and clear it */
5:
    lw a1, 0x14(tp)
    andi a1, a1, 2
    beqz a1, 5b
    lw a1, 0x10(tp)

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

.globl spi_set_config
spi_set_config:
    sw a0, 0x24(tp)
    ret
