#include <stdint.h>
#include <stdbool.h>

inline static void gamepad_enable() {
    asm volatile ( "sb %[value], 0xc0(tp)" : : [value] "r" (1));
}

// Note that peripheral 1 and 2 are switched compared to the labelling on the PMOD
// This API reverses that switch
inline static bool gamepad_1_present() {
    int present;
    asm volatile ( "lbu %[value], 0xc3(tp)" : [value] "=r" (present));
    return present;
}

inline static bool gamepad_2_present() {
    int present;
    asm volatile ( "lbu %[value], 0xc2(tp)" : [value] "=r" (present));
    return present;
}

typedef struct gamepad_state {
    union {
        struct {
            uint16_t r : 1;
            uint16_t l : 1;
            uint16_t x : 1;
            uint16_t a : 1;
            uint16_t right : 1;
            uint16_t left : 1;
            uint16_t down : 1;
            uint16_t up : 1;
            uint16_t start : 1;
            uint16_t select : 1;
            uint16_t y : 1;
            uint16_t b : 1;
        };
        uint16_t state;
    };
} gamepad_state_t;

inline static gamepad_state_t gamepad_1_state() {
    int state;
    asm volatile ( "lhu %[value], 0xc6(tp)" : [value] "=r" (state));
    gamepad_state_t g_state;
    g_state.state = state;
    return g_state;
}

inline static gamepad_state_t gamepad_2_state() {
    int state;
    asm volatile ( "lhu %[value], 0xc4(tp)" : [value] "=r" (state));
    gamepad_state_t g_state;
    g_state.state = state;
    return g_state;
}

inline static bool gamepad_1_r() {
    int pressed;
    asm volatile ( "lbu %[value], 0xec(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_l() {
    int pressed;
    asm volatile ( "lbu %[value], 0xed(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_x() {
    int pressed;
    asm volatile ( "lbu %[value], 0xee(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_a() {
    int pressed;
    asm volatile ( "lbu %[value], 0xef(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_right() {
    int pressed;
    asm volatile ( "lbu %[value], 0xf0(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_left() {
    int pressed;
    asm volatile ( "lbu %[value], 0xf1(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_down() {
    int pressed;
    asm volatile ( "lbu %[value], 0xf2(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_up() {
    int pressed;
    asm volatile ( "lbu %[value], 0xf3(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_start() {
    int pressed;
    asm volatile ( "lbu %[value], 0xf4(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_select() {
    int pressed;
    asm volatile ( "lbu %[value], 0xf5(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_y() {
    int pressed;
    asm volatile ( "lbu %[value], 0xf6(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_1_b() {
    int pressed;
    asm volatile ( "lbu %[value], 0xf7(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_r() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe0(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_l() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe1(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_x() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe2(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_a() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe3(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_right() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe4(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_left() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe5(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_down() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe6(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_up() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe7(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_start() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe8(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_select() {
    int pressed;
    asm volatile ( "lbu %[value], 0xe9(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_y() {
    int pressed;
    asm volatile ( "lbu %[value], 0xea(tp)" : [value] "=r" (pressed));
    return pressed;
}

inline static bool gamepad_2_b() {
    int pressed;
    asm volatile ( "lbu %[value], 0xeb(tp)" : [value] "=r" (pressed));
    return pressed;
}
