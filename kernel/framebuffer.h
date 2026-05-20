#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include "../limine.h"
#include "types.h"

// BGRA Color Format
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} Color;

static inline uint32_t color_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // RGBA to 0xAARRGGBB
    return ((uint32_t)a << 24)
            | ((uint32_t)r << 16)
            | ((uint32_t)g << 8)
            | ((uint32_t)b << 0);
}

static inline void draw_pixel(struct limine_framebuffer *fb, uint32_t x, uint32_t y, uint32_t color) {
    uint32_t pixels_per_row = fb->pitch / 4;
    volatile uint32_t *pixel = (uint32_t *) fb->address;

    uint64_t offset = y * pixels_per_row + x;
    pixel[offset] = color;
}

static inline void draw_rectangle(struct limine_framebuffer *fb, Position2D *starting_position, Position2D *ending_position, uint32_t color) {
    volatile uint32_t *pixel = (uint32_t *) fb->address;
    uint32_t pixel_per_row = fb->pitch / 4;

    for (uint64_t i = 0; i < (ending_position->y - starting_position->y); i++) {

        uint32_t current_y = starting_position->y + i;
        uint32_t row_start = current_y * pixel_per_row;

        for (uint64_t j = 0; j < (ending_position->x - starting_position->x); j++) {
            uint32_t current_x = starting_position->x + j;

            pixel[row_start + current_x] = color;
        }
    }
}
#endif