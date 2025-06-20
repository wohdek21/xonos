#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limine.h>

// Структура шрифта PSF
typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
} __attribute__((packed)) psf1_header_t;

typedef struct {
    void* address;
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
    uint16_t bpp;
} framebuffer_t;

// Инициализация framebuffer и шрифта
void fb_init(struct limine_framebuffer *fb, void* psf_font);

// Примитивы
void fb_put_pixel(uint64_t x, uint64_t y, uint32_t color);
void fb_draw_char(uint64_t x, uint64_t y, char c, uint32_t fg);
void fb_write_text(uint64_t x, uint64_t y, const char* s, uint32_t fg);
void fb_draw_rect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color);
void fb_draw_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color);
void fb_draw_bitmap(uint64_t x, uint64_t y, const uint8_t* bmp, uint64_t w, uint64_t h, uint32_t color);
