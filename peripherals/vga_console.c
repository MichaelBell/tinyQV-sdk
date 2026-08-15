/* VGA console peripheral (peripheral 13 on Asteroids) */

#include <gpio.h>

#include "vga_console.h"

void vga_console_start() {
    // Set all outputs to regular mode (not debug)
    enable_all_outputs();

    // Set all outputs to peripheral 13
    for (int i = 0; i < 8; ++i) set_gpio_func(i, 13);
}

void vga_console_set_text(int start_pos, const char* text, bool alt_colour) {
    uint8_t colour = alt_colour ? 0x80 : 0;
    volatile uint8_t* vga_text = (volatile uint8_t*)0x8000340;
    for (int i = start_pos; i < 30 && *text; ++i) {
        vga_text[i] = (uint8_t)*text++ | colour;
    }
}

void vga_console_reset() {
    volatile uint8_t* vga_text = (volatile uint8_t*)0x8000340;
    for (int i = 0; i < 30; ++i) {
        vga_text[i] = 0x20;
    }
}
