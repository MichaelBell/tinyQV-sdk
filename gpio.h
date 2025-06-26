#pragma once

#include <stdint.h>

inline static void set_outputs(uint32_t value)
{
    asm( "sw %[value], 0x40(tp)" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_outputs()
{
    uint32_t value;
    asm( "lw %[value], 0x40(tp)" : [value] "=r" (value) : : "memory");
    return value;
}

inline static void gpio_on(int gpio)
{
    uint32_t val = get_outputs() | (1 << gpio);
    set_outputs(val);
}

inline static void gpio_off(int gpio)
{
    uint32_t val = get_outputs() & (~(1 << gpio));
    set_outputs(val);
}

inline static uint32_t get_inputs()
{
    uint32_t value;
    asm volatile ( "lw %[value], 0x44(tp)" : [value] "=r" (value) : : "memory");
    return value;
}

// Note iceFUN only
inline static void set_leds(uint32_t value)
{
    asm( "sw %[value], 8(tp)" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_leds()
{
    uint32_t value;
    asm( "lw %[value], 8(tp)" : [value] "=r" (value) : : "memory");
    return value;
}

inline static void set_debug_sel(uint32_t value)
{
    asm( "sw %[value], 0xc(tp)" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_debug_sel()
{
    uint32_t value;
    asm( "lw %[value], 0xc(tp)" : [value] "=r" (value) : : "memory");
    return value;
}

inline static void set_gpio_func(uint32_t gpio, uint32_t func)
{
    volatile uint32_t* gpio_func_ptr = (volatile uint32_t*)0x8000060 + gpio;
    *gpio_func_ptr = func;
}

inline static void set_gpio_sel(uint32_t value) {
    set_debug_sel(value);
    for (uint32_t i = 0; i < 8; ++i) {
        set_gpio_func(i, value & 1);
        value >>= 1;
    }
}