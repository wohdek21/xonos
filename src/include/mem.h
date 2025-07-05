#pragma once
#include <stddef.h>
#include <stdint.h>
#include "limine.h"

typedef struct {
    uint64_t base;
    uint64_t length;
    int free;
} mem_block_t;

void mem_init(struct limine_memmap_response* memmap);
void* kmalloc(size_t size);
void kfree(void* ptr);
size_t mem_get_block_count(void);
void mem_get_block(size_t idx, mem_block_t* out);
void mem_info(void);