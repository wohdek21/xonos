#include "mem.h"
#include "limine.h"
#include "framebuffer.h"
#include <stddef.h>

#define MAX_BLOCKS 1024

static mem_block_t blocks[MAX_BLOCKS];
static size_t block_count = 0;

// используем собственный cursor_x / cursor_y
static int cursor_x = 0;
static int cursor_y = 0;

// печать строки
static void fb_print(const char* s) {
    while (*s) {
        char c[2] = {*s, 0};
        fb_write_text(cursor_x, cursor_y, c, 0x00FF00);
        cursor_x += 8;
        if (cursor_x >= 640) {
            cursor_x = 0;
            cursor_y += 16;
        }
        s++;
    }
}

// десятичное число
static void fb_print_dec(uint64_t val) {
    char buf[21];
    int i = 0;

    if (val == 0) {
        buf[i++] = '0';
    } else {
        while (val && i < 20) {
            buf[i++] = '0' + (val % 10);
            val /= 10;
        }
    }

    for (int j = i - 1; j >= 0; j--) {
        char c[2] = {buf[j], 0};
        fb_write_text(cursor_x, cursor_y, c, 0x00FF00);
        cursor_x += 8;
    }
}

// шестнадцатеричное число
static void fb_print_hex(uint64_t val) {
    char buf[17];
    int i = 0;

    if (val == 0) {
        fb_write_text(cursor_x, cursor_y, "0", 0x00FF00);
        cursor_x += 8;
        return;
    }

    while (val && i < 16) {
        uint8_t digit = val & 0xF;
        buf[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        val >>= 4;
    }

    // печатаем в обратном порядке
    for (int j = i - 1; j >= 0; j--) {
        char c[2] = {buf[j], 0};
        fb_write_text(cursor_x, cursor_y, c, 0x00FF00);
        cursor_x += 8;
    }
}

void mem_init(struct limine_memmap_response* memmap) {
    cursor_x = 0;
    cursor_y += 16;

    fb_write_text(cursor_x, cursor_y, "mem_init: ", 0x00FF00);
    cursor_x += 9 * 8;

    block_count = 0;
    for (uint64_t i = 0; i < memmap->entry_count && block_count < MAX_BLOCKS; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            blocks[block_count].base = entry->base;
            blocks[block_count].length = entry->length;
            blocks[block_count].free = 1;

            /*cursor_x = 0;
            cursor_y += 16;
            fb_print("block ");
            fb_print_dec(block_count);
            fb_print(": base=0x");
            fb_print_hex(entry->base);
            fb_print(" len=0x");
            fb_print_hex(entry->length);*/

            block_count++;
        }
    }

    cursor_x = 0;
    cursor_y += 16;
    fb_print("total blocks: ");
    fb_print_dec(block_count);
}

void* kmalloc(size_t size) {
    for (size_t i = 0; i < block_count; i++) {
        if (blocks[i].free && blocks[i].length >= size) {
            void* ptr = (void*)(uintptr_t)blocks[i].base;
            blocks[i].base += size;
            blocks[i].length -= size;
            if (blocks[i].length == 0)
                blocks[i].free = 0;
            return ptr;
        }
    }
    return NULL;
}

void kfree(void* ptr) {
    (void)ptr; // заглушка
}

void mem_info(void) {
    cursor_x = 0;
    cursor_y += 16;
    fb_print("mem_info:\n");

    for (size_t i = 0; i < block_count; i++) {
        mem_block_t* b = &blocks[i];
        cursor_x = 0;
        cursor_y += 16;

        fb_print("Block ");
        fb_print_dec(i);
        fb_print(": base=0x");
        fb_print_hex(b->base);
        fb_print(" len=0x");
        fb_print_hex(b->length);
        fb_print(" free=");
        fb_print_dec(b->free);
    }
}

size_t mem_get_block_count(void) {
    return block_count;
}

void mem_get_block(size_t idx, mem_block_t* out) {
    if (idx < block_count && out) {
        *out = blocks[idx];
    }
}
