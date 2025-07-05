#include "error.h"
#include "framebuffer.h"

void kernel_panic(const char* message) {
    fb_draw_rect(0, 0, 640, 480, 0x000000);
    fb_write_text(0, 0, "KERNEL PANIC:", 0xFF0000);
    fb_write_text(0, 16, message, 0xFFFFFF);

    for (;;) {
        asm volatile("cli; hlt");
    }
}