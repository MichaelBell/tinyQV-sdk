#pragma once

#include <stdint.h>
#include <stdbool.h>

inline static uint32_t get_mtime()
{
    uint32_t value;
    asm volatile ( "lw %[value], -0x100(x0)" : [value] "=r" (value));
    return value;
}

inline static void set_mtime(uint32_t value)
{
    asm volatile ( "sw %[value], -0x100(x0)" : : [value] "r" (value));
}

inline static void set_mtimecmp(uint32_t value)
{
    asm volatile ( "sw %[value], -0xfc(x0)" : : [value] "r" (value));
}

inline static uint32_t get_mtimecmp()
{
    uint32_t value;
    asm volatile ( "lw %[value], -0xfc(x0)" : [value] "=r" (value));
    return value;
}

inline static void enable_timer_interrupt()
{
    asm volatile ("csrs mie, %[value]" : : [value] "r" (0x80));
}

inline static void disable_timer_interrupt()
{
    asm volatile ("csrc mie, %[value]" : : [value] "r" (0x80));
}

// Add a callback to be made after a certain amount of time.
// Up to eight alarms can be set simultaneously.
// This has some overhead so is best suited when you need timers
// measure in milliseconds.
// set_alarm must not be called from interrupt contexts other than the timer interrupt.
typedef void(*alarm_callback_t)(void*);
bool set_alarm(int ms_delay, alarm_callback_t callback, void* handle);

// As an alternative to the alarm interface, define a function
//   void tqv_timer_interrupt(void)
// in your code, and either control mtimecmp directly or use this helper 
// to trigger an interrupt after a delay in microseconds.
// Note that in your interrupt handler you must either disable the interrupt
// or set mtimecmp to a new value.
void trigger_timer_interrupt(int us_delay);
