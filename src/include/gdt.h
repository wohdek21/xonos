#pragma once
#include <stdint.h>

// GDT entry (8 байт)
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

// GDT descriptor (указатель для lgdt)
struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// функция инициализации
void gdt_init(void);
