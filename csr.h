#include <stdint.h>

inline static uint32_t read_cycle() {
    int r;
    asm volatile ("rdcycle %0" :
         [r] "=r" (r) 
         );
    return r;
}

inline static uint32_t read_time() {
    int r;
    asm volatile ("rdtime %0" :
         [r] "=r" (r) 
         );
    return r;
}


inline static uint32_t read_instret() {
    int r;
    asm volatile ("rdinstret %0" :
         [r] "=r" (r) 
         );
    return r;
}

