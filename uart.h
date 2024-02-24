#pragma once

#include <stdint.h>
#include <stddef.h>

void uart_putc(int c);
void uart_puts(const char* s);
void uart_put_buffer(const char* c, int len);
int uart_is_char_available(void);
int uart_getc(void);
int uart_printf(const char *fmt, ...);

void debug_uart_putc(int c);
void debug_uart_put_buffer(const char* c, int len);
