#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <limine.h>
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "irq.h"
#include "framebuffer.h"
#include "time.h"
#include "keyboard.h"
#include "mem.h"
#include "paging.h"


__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

extern uint8_t _binary_font_start;
uint64_t counter = 0;
extern void print_prompt(void);

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

void int_to_str(uint64_t n, char* buf) {
    char tmp[21];
    int i = 0;
    if (n == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while (n > 0) {
        tmp[i++] = '0' + (n % 10);
        n /= 10;
    }
    for (int j = 0; j < i; j++)
        buf[j] = tmp[i - j - 1];
    buf[i] = '\0';
}

static void hcf(void) {
    for (;;) asm ("hlt");
}

void kmain(void) {
    if (!LIMINE_BASE_REVISION_SUPPORTED || framebuffer_request.response == NULL
        || framebuffer_request.response->framebuffer_count < 1
        || memmap_request.response == NULL) {
        hcf();
    }

    struct limine_framebuffer* fb = framebuffer_request.response->framebuffers[0];
    fb_init(fb, &_binary_font_start); 
    mem_init(memmap_request.response);
    paging_init(mem_total());

    uint64_t test_val = 0xCAFEBABE;
    uint64_t phys = paging_get_phys((uint64_t)&test_val);
    
    char buf[32];
    int_to_str(phys, buf);
    fb_write_text(0, 64, "Test phys addr: ", 0x00FF00);
    fb_write_text(160, 64, buf, 0x00FF00);
    

    gdt_init();
    idt_init();
    pic_remap(0x20, 0x28);
    irq_init();
    pit_init(1000);
    asm volatile("sti");

    print_prompt();

    for (;;);
}

void tick(void) {
    counter++;
}
