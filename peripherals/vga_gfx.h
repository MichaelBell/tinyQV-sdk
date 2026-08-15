/* Graphics support functions for VGA gfx peripheral (peripheral 9 on Asteroids) */

#include <stdint.h>

extern uint32_t* vga_gfx_front_buffer;
extern uint32_t* vga_gfx_back_buffer;

// Setup VGA output
void vga_gfx_start();

// Flip buffers
void vga_gfx_flip();

// Fast memset of buffer to a solid colour
void vga_gfx_clear(int colour, uint32_t* buffer);

// Draw a solid box
void vga_gfx_box(int colour, uint32_t* buffer, int x, int y, int w, int h);

// Draw a horizontal line
void vga_gfx_hline(int colour, uint32_t* buffer, int x, int y, int w);

// Draw a vertical line
void vga_gfx_vline(int colour, uint32_t* buffer, int x, int y, int h);

// Copy all of source onto the frame at (x,y).
// from is padded so that each line starts aligned at a word boundary
// (i.e. stride = h rounded up to next multiple of 16)
void vga_gfx_simple_blit(uint32_t* buffer, const uint32_t* source, int x, int y, int w, int h);

inline static void vga_gfx_set_colour1(uint8_t colour) {
    asm volatile ( "sb %[colour], 0x245(tp)" : : [colour] "r" (colour));
}
inline static void vga_gfx_set_colour2(uint8_t colour) {
    asm volatile ( "sb %[colour], 0x246(tp)" : : [colour] "r" (colour));
}
inline static void vga_gfx_set_colour3(uint8_t colour) {
    asm volatile ( "sb %[colour], 0x247(tp)" : : [colour] "r" (colour));
}

inline static void vga_gfx_wait_for_vsync() {
    uint8_t yhigh;
    do {
        asm volatile ( "lbu %[value], 0x243(tp)" : [value] "=r" (yhigh));
    } while (yhigh != 16);
}
