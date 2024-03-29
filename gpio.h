#pragma once

#include <stdint.h>

inline static void set_outputs(uint32_t value)
{
    asm( "sw %[value], (tp)" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_outputs()
{
    uint32_t value;
    asm( "lw %[value], (tp)" : [value] "=r" (value) : : "memory");
    return value;
}

inline static uint32_t get_inputs()
{
    uint32_t value;
    asm volatile ( "lw %[value], 4(tp)" : [value] "=r" (value) : : "memory");
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

inline static void set_gpio_sel(uint32_t value)
{
    asm( "sw %[value], 0xc(tp)" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_gpio_sel()
{
    uint32_t value;
    asm( "lw %[value], 0xc(tp)" : [value] "=r" (value) : : "memory");
    return value;
}
