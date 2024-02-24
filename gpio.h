#pragma once

#include <stdint.h>

inline static void set_outputs(uint32_t value)
{
    //asm( "sw %[value], (tp)" : : [value] "r" (value) : "memory");
    asm( ".insn css 2, 7, %[value], 0" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_outputs()
{
    uint32_t value;
    //asm( "lw %[value], (tp)" : [value] "=r" (value) : : "memory");
    asm( ".insn ci 2, 3, %[value], 0" : [value] "=r" (value) : : "memory");
    return value;
}

inline static uint32_t get_inputs()
{
    uint32_t value;
    //asm volatile ( "lw %[value], 4(tp)" : [value] "=r" (value) : : "memory");
    asm volatile ( ".insn ci 2, 3, %[value], 4" : [value] "=r" (value) : : "memory");
    return value;
}

// Note iceFUN only
inline static void set_leds(uint32_t value)
{
    asm( ".insn css 2, 7, %[value], 8" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_leds()
{
    uint32_t value;
    asm( ".insn ci 2, 3, %[value], 8" : [value] "=r" (value) : : "memory");
    return value;
}

inline static void set_gpio_sel(uint32_t value)
{
    asm( ".insn css 2, 7, %[value], 0xc" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_gpio_sel()
{
    uint32_t value;
    asm( ".insn ci 2, 3, %[value], 0xc" : [value] "=r" (value) : : "memory");
    return value;
}
