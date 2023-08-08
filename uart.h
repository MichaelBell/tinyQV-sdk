#pragma once

#include <stdint.h>
#include <stddef.h>

void uart_putc(int c);
int uart_is_char_available(void);
int uart_getc(void);
int uart_printf(const char *fmt, ...);
