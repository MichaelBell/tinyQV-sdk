.set UART_BUF_SIZE, 64

.section .text
.globl isr_uart_writable  # Note no stack, and only use a0 and a1, return address in s1
isr_uart_writable:
    # Load read and write pointers.  If equal, nothing more to send
    lw a1, uart_write_ptr
    lw a0, uart_read_ptr
    beq a0, a1, 1f

    # Send the byte to the UART
    lb a1, (a0)
    .insn css 2, 7, a1, 0x10

    # Increment read pointer and compare with end of buffer
    addi a0, a0, 1
    la a1, uart_buffer + UART_BUF_SIZE
    beq a0, a1, 2f
3:
    # Store the incremented read pointer
    sw a0, uart_read_ptr, a1
    jalr x0, (s1)
2:
    la a0, uart_buffer
    j 3b

1:
    # Nothing more to send, disable interrupt
    li a0, 0x80000
    csrc mie, a0
    jalr x0, (s1)

.globl uart_putc
uart_putc:
    la a2, uart_buffer + UART_BUF_SIZE

    # Disable interrupts as uart buffer ptrs aren't locked
    li a3, 0x8
    csrc mstatus, a3

    # Load the write pointer and store the byte
    lw a1, uart_write_ptr
    sb a0, (a1)

    # Increment the write pointer
    addi a1, a1, 1
    beq a1, a2, 1f
2:
    # Check if we've caught up with the read pointer
    lw a4, uart_read_ptr
    beq a4, a1, 3f

5:
    # Store the incremented write pointer, re-enable interrupts
    sw a1, uart_write_ptr, a0
    li a1, 0x80000
    csrs mie, a1
    csrs mstatus, a3
    ret
1:
    la a1, uart_buffer
    j 2b

3:
    # Re-enable interrupts and wait for read pointer to change
    csrs mstatus, a3
4:
    lw a2, uart_read_ptr
    beq a2, a4, 4b

    # Disable interrupts to protect re-enabling the writable interrup
    csrc mstatus, a3
    j 5b
