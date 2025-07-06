#pragma once
#include <stdint.h>
#include <stddef.h>
#include <limine.h>

void mem_init(struct limine_memmap_response* memmap);
void* alloc_frame(void);
void free_frame(void* frame);
uint64_t mem_total(void);
