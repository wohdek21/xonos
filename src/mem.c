#include "mem.h"
#include <string.h>

#define PAGE_SIZE 0x1000
#define MAX_FRAMES 65536  // до 256 MiB
static uint8_t bitmap[MAX_FRAMES / 8];
static size_t total_frames = 0;
static size_t used_frames = 0;

#define BITMAP_SET(idx) (bitmap[(idx)/8] |=  (1 << ((idx)%8)))
#define BITMAP_CLR(idx) (bitmap[(idx)/8] &= ~(1 << ((idx)%8)))
#define BITMAP_TST(idx) (bitmap[(idx)/8] &   (1 << ((idx)%8)))

static size_t find_free_frame() {
    for (size_t i = 0; i < total_frames; i++) {
        if (!BITMAP_TST(i)) return i;
    }
    return (size_t)-1;
}

void mem_init(struct limine_memmap_response* memmap) {
    memset(bitmap, 0, sizeof(bitmap));
    total_frames = 0;
    used_frames = 0;

    uint64_t max_addr = 0;

    for (size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            uint64_t top = entry->base + entry->length;
            if (top > max_addr) max_addr = top;
        }
    }

    total_frames = max_addr / PAGE_SIZE;
    if (total_frames > MAX_FRAMES) total_frames = MAX_FRAMES;

    // Резервируем bitmap и ядро
    extern uint64_t _kernel_end;
    size_t reserved = ((uint64_t)&_kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;
    for (size_t i = 0; i < reserved; i++) {
        BITMAP_SET(i);
        used_frames++;
    }

    // Резерв bitmap тоже
    uintptr_t bmp_base = (uintptr_t)&bitmap;
    size_t bmp_pages = (sizeof(bitmap) + PAGE_SIZE - 1) / PAGE_SIZE;
    for (size_t i = 0; i < bmp_pages; i++) {
        BITMAP_SET((bmp_base / PAGE_SIZE) + i);
        used_frames++;
    }
}

void* alloc_frame(void) {
    size_t idx = find_free_frame();
    if (idx == (size_t)-1) return NULL;
    BITMAP_SET(idx);
    used_frames++;
    return (void*)(idx * PAGE_SIZE);
}

void free_frame(void* frame) {
    size_t addr = (size_t)(uintptr_t)frame;
    size_t idx = addr / PAGE_SIZE;
    if (idx < total_frames && BITMAP_TST(idx)) {
        BITMAP_CLR(idx);
        used_frames--;
    }
}

uint64_t mem_total(void) {
    return total_frames * PAGE_SIZE;
}
