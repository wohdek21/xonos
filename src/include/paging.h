#pragma once
#include <stdint.h>
#include <stddef.h>

#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_USER    0x4

void paging_init(uint64_t total_phys);
void paging_map_identity(uint64_t base, size_t size, uint64_t flags);
void paging_map_kernel(uint64_t phys_base, uint64_t virt_base, size_t size, uint64_t flags);
void paging_unmap_page(uint64_t virt);
uint64_t paging_get_phys(uint64_t virt);
