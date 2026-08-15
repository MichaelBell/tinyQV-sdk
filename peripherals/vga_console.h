/* VGA console peripheral (peripheral 13 on Asteroids) */

#include <stdint.h>
#include <stdbool.h>

// Setup VGA output
void vga_console_start();

// Clear the text (it is uninitialized on reset)
void vga_console_reset();

// Write text to position, wrapping rows.  The rows are 10 characters long.
void vga_console_set_text(int start_pos, const char* text, bool alt_colour);

inline static void vga_console_set_bgcolour(uint8_t colour) {
    asm volatile ( "sb %[colour], 0x370(tp)" : : [colour] "r" (colour));
}
inline static void vga_console_set_colour1(uint8_t colour) {
    asm volatile ( "sb %[colour], 0x371(tp)" : : [colour] "r" (colour));
}
inline static void vga_console_set_colour2(uint8_t colour) {
    asm volatile ( "sb %[colour], 0x372(tp)" : : [colour] "r" (colour));
}
