#include "framebuffer.h"
#include <stdlib.h>


static framebuffer_t fb;
static psf1_header_t* font;
static uint8_t* glyph_buffer;

void fb_init(struct limine_framebuffer *lfb, void* psf_font) {
    fb.address = lfb->address;
    fb.pitch = lfb->pitch;
    fb.width = lfb->width;
    fb.height = lfb->height;
    fb.bpp = lfb->bpp;

    font = (psf1_header_t*)psf_font;
    glyph_buffer = (uint8_t*)((uint8_t*)psf_font + sizeof(psf1_header_t));
}

void fb_put_pixel(uint64_t x, uint64_t y, uint32_t color) {
    if (x >= fb.width || y >= fb.height) return;
    uint32_t* pixel = (uint32_t*)((uint8_t*)fb.address + fb.pitch * y + x * 4);
    *pixel = color;
}

void fb_draw_char(uint64_t x, uint64_t y, char c, uint32_t fg) {
    uint8_t* glyph = glyph_buffer + (c * font->charsize);
    for (uint8_t i = 0; i < font->charsize; i++) {
        uint8_t line = glyph[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (line & (0x80 >> j)) {
                fb_put_pixel(x + j, y + i, fg);
            }
        }
    }
}

void fb_write_text(uint64_t x, uint64_t y, const char* s, uint32_t fg) {
    while (*s) {
        fb_draw_char(x, y, *s++, fg);
        x += 8;
    }
}

void fb_draw_rect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color) {
    for (uint64_t i = 0; i < h; i++) {
        for (uint64_t j = 0; j < w; j++) {
            fb_put_pixel(x + j, y + i, color);
        }
    }
}

/*void fb_draw_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color) {
    int dx = abs((int)x1 - (int)x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs((int)y1 - (int)y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        fb_put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}*/

void fb_draw_bitmap(uint64_t x, uint64_t y, const uint8_t* bmp, uint64_t w, uint64_t h, uint32_t color) {
    for (uint64_t i = 0; i < h; i++) {
        for (uint64_t j = 0; j < w; j++) {
            if (bmp[i * w + j]) {
                fb_put_pixel(x + j, y + i, color);
            }
        }
    }
}
