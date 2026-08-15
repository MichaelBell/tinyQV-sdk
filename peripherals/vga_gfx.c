/* Graphics support functions for VGA gfx peripheral (peripheral 9 on Asteroids) */

#include <mul.h>
#include <gpio.h>
#include <csr.h>
#include "vga_gfx.h"

#define WIDTH 256
#define HEIGHT 192

static uint32_t framebuffer1[16*HEIGHT];
static uint32_t framebuffer2[16*HEIGHT];
uint32_t* vga_gfx_front_buffer = framebuffer1;
uint32_t* vga_gfx_back_buffer = framebuffer2;

#define MIN(A,B) ((A) < (B) ? (A) : (B))

void vga_gfx_flip() {
    vga_gfx_wait_for_vsync();
    uint32_t* tmp = vga_gfx_front_buffer;
    vga_gfx_front_buffer = vga_gfx_back_buffer;
    vga_gfx_back_buffer = tmp;
}

void vga_gfx_box(int colour, uint32_t* buffer, int x, int y, int w, int h) {
    if (x < 0 || x >= WIDTH) return;
    if (y < 0 || y >= HEIGHT) return;
    const int start_x_bit = (x & 0xf) << 1;
    const int start_x_word = (x & 0xf0) >> 4;
    const int end_x = x + w - 1;
    const int end_x_bit = end_x >= 256 ? 30 : (end_x & 0xf) << 1;
    const int end_x_word = end_x >= 256 ? 0xf : end_x >> 4;
    const int max_y = MIN(y + h, HEIGHT);
    const uint32_t start_mask = 0xffffffff << start_x_bit;
    const uint32_t end_mask = 0xffffffff >> (30 - end_x_bit);
    colour = mul32x16(0x55555555, colour);
    if (end_x_word == start_x_word) {
        const uint32_t mask = start_mask & end_mask;
        colour &= mask;
        for (int i = y; i < max_y; ++i) {
            buffer[i*16 + start_x_word] &= ~mask;
            buffer[i*16 + start_x_word] |= colour;
        }
    } else {
        const uint32_t start_colour = colour & start_mask;
        const uint32_t end_colour = colour & end_mask;
        if (start_x_word + 1 == end_x_word) {
            for (int i = y; i < max_y; ++i) {
                buffer[i*16 + start_x_word] &= ~start_mask;
                buffer[i*16 + start_x_word] |= start_colour;
                buffer[i*16 + end_x_word] &= ~end_mask;
                buffer[i*16 + end_x_word] |= end_colour;
            }
        } else {
            for (int i = y; i < max_y; ++i) {
                buffer[i*16 + start_x_word] &= ~start_mask;
                buffer[i*16 + start_x_word] |= start_colour;
                for (int j = start_x_word + 1; j < end_x_word; ++j) {
                    buffer[i*16 + j] = colour;
                }
                buffer[i*16 + end_x_word] &= ~end_mask;
                buffer[i*16 + end_x_word] |= end_colour;
            }
        }
    }
}

void vga_gfx_hline(int colour, uint32_t* buffer, int x, int y, int w) {
    const int start_x_bit = (x & 0xf) << 1;
    const int start_x_word = (x & 0xf0) >> 4;
    const int end_x = x + w - 1;
    const int end_x_bit = (end_x & 0xf) << 1;
    const int end_x_word = (end_x & 0xf0) >> 4;
    const uint32_t start_mask = 0xffffffff << start_x_bit;
    const uint32_t end_mask = 0xffffffff >> (30 - end_x_bit);
    colour = mul32x16(0x55555555, colour);
    if (end_x_word == start_x_word) {
        const uint32_t mask = start_mask & end_mask;
        colour &= mask;
        buffer[y*16 + start_x_word] &= ~mask;
        buffer[y*16 + start_x_word] |= colour;
    } else {
        const uint32_t start_colour = colour & start_mask;
        const uint32_t end_colour = colour & end_mask;
        buffer[y*16 + start_x_word] &= ~start_mask;
        buffer[y*16 + start_x_word] |= start_colour;
        buffer[y*16 + end_x_word] &= ~end_mask;
        buffer[y*16 + end_x_word] |= end_colour;
        for (int j = start_x_word + 1; j < end_x_word; ++j) {
            buffer[y*16 + j] = colour;
        }
    }
}

void vga_gfx_vline(int colour, uint32_t* buffer, int x, int y, int h) {
    const int x_bit = (x & 0xf) << 1;
    const int x_word = (x & 0xf0) >> 4;
    const uint32_t mask = 0x3 << x_bit;
    colour <<= x_bit;
    for (int i = y; i < y+h; ++i) {
        buffer[i*16 + x_word] &= ~mask;
        buffer[i*16 + x_word] |= colour;
    }   
}

// Copy all of from onto the frame at (x,y).
// from is padded so that each line starts aligned at a word boundary
// (i.e. stride = h rounded up to next multiple of 16)
void vga_gfx_simple_blit(uint32_t* buffer, const uint32_t* from, int x, int y, int w, int h) {
    if (x < 0 || x >= WIDTH) return;
    if (y < 0 || y >= HEIGHT) return;
    const int start_x_bit = (x & 0xf) << 1;
    const int start_x_word = (x & 0xf0) >> 4;
    const int end_x = x + w - 1;
    const int end_x_bit = end_x >= 256 ? 30 : (end_x & 0xf) << 1;
    const int end_x_word = end_x >= 256 ? 0xf : end_x >> 4;
    const int max_y = MIN(y + h, HEIGHT);
    const uint32_t start_mask = 0xffffffff << start_x_bit;
    const uint32_t end_mask = 0xffffffff >> (30 - end_x_bit);
    if (end_x_word == start_x_word) {
        const uint32_t mask = start_mask & end_mask;
        for (int i = y; i < max_y; ++i) {
            uint32_t data = *from++;
            data <<= start_x_bit;
            data &= mask;
            buffer[i*16 + start_x_word] &= ~mask;
            buffer[i*16 + start_x_word] |= data;
        }
    } else if (start_x_bit == 0) {
        for (int i = y; i < max_y; ++i) {
            for (int j = start_x_word; j < end_x_word; ++j) {
                buffer[i*16 + j] = *from++;
            }
            uint32_t data = *from++;
            data &= end_mask;
            buffer[i*16 + end_x_word] &= ~end_mask;
            buffer[i*16 + end_x_word] |= data;
        }
    } else {
        const uint32_t remaining_bit = 32 - start_x_bit;
        for (int i = y; i < max_y; ++i) {
            uint32_t data = *from++;
            uint32_t remaining_data = data >> remaining_bit;

            buffer[i*16 + start_x_word] &= ~start_mask;
            buffer[i*16 + start_x_word] |= data << start_x_bit;
            
            for (int j = start_x_word + 1; j < end_x_word; ++j) {
                data = *from++;
                buffer[i*16 + j] = (data << start_x_bit) | remaining_data;
                remaining_data = data >> remaining_bit;
            }

            if (start_x_bit <= end_x_bit) data = *from++;

            data = (data << start_x_bit) | remaining_data;
            buffer[i*16 + end_x_word] &= ~end_mask;
            buffer[i*16 + end_x_word] |= data & end_mask;
        }
    }
}
