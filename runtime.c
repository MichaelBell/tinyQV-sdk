// Borrowed from the Pico SDK
// Copyright (c) 2020 Raspberry Pi (Trading) Ltd.

#include <sys/time.h>
#include "uart.h"

extern char __StackLimit; /* Set by linker.  */
extern char __HeapStart; /* Set by linker.  */
char* __nano_heap_end = &__HeapStart;

extern uint32_t __sbss_start__;
extern uint32_t __sbss_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

void _bss_init(void) {
    for (uint32_t* ptr = &__sbss_start__; ptr < &__sbss_end__; ptr++) *ptr = 0;
    for (uint32_t* ptr = &__bss_start__; ptr < &__bss_end__; ptr++) *ptr = 0;
}

void *_sbrk(int incr) {
    char *prev_heap_end;

    prev_heap_end = __nano_heap_end;
    char *next_heap_end = __nano_heap_end + incr;

    //uart_printf("SBRK: %p -> %p\r\n", __nano_heap_end, next_heap_end);

    if (next_heap_end > (&__StackLimit)) {
        return (char *) -1;
    }

    __nano_heap_end = next_heap_end;
    return (void *) prev_heap_end;
}

int _gettimeofday (struct timeval *__restrict tv, void *__restrict tz) {
    return 0;
}
