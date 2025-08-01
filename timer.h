#pragma once

#include <stdint.h>

inline static void set_mtime(uint32_t value)
{
    asm( "sw %[value], -0x100(x0)" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_mtime()
{
    uint32_t value;
    asm( "lw %[value], -0x100(x0)" : [value] "=r" (value) : : "memory");
    return value;
}

inline static void set_mtimecmp(uint32_t value)
{
    asm( "sw %[value], -0xfc(x0)" : : [value] "r" (value) : "memory");
}

inline static uint32_t get_mtimecmp()
{
    uint32_t value;
    asm( "lw %[value], -0xfc(x0)" : [value] "=r" (value) : : "memory");
    return value;
}
