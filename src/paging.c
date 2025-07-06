// paging.c
#include "paging.h"
#include "mem.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_USER    0x4

#define PAGE_SIZE 0x1000

static uint64_t* pml4;

static uint64_t* alloc_page_table() {
    void* page = alloc_frame();
    if (!page) return NULL;
    memset(page, 0, PAGE_SIZE);
    return (uint64_t*)page;
}

static void map_page(uint64_t virt, uint64_t phys, uint64_t flags) {
    size_t pml4_index = (virt >> 39) & 0x1FF;
    size_t pdpt_index = (virt >> 30) & 0x1FF;
    size_t pd_index   = (virt >> 21) & 0x1FF;
    size_t pt_index   = (virt >> 12) & 0x1FF;

    uint64_t* pdpt;
    if (!(pml4[pml4_index] & PAGE_PRESENT)) {
        pdpt = alloc_page_table();
        pml4[pml4_index] = (uint64_t)pdpt | flags;
    } else {
        pdpt = (uint64_t*)(pml4[pml4_index] & ~0xFFFULL);
    }

    uint64_t* pd;
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        pd = alloc_page_table();
        pdpt[pdpt_index] = (uint64_t)pd | flags;
    } else {
        pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFFULL);
    }

    uint64_t* pt;
    if (!(pd[pd_index] & PAGE_PRESENT)) {
        pt = alloc_page_table();
        pd[pd_index] = (uint64_t)pt | flags;
    } else {
        pt = (uint64_t*)(pd[pd_index] & ~0xFFFULL);
    }

    pt[pt_index] = phys | flags;
}

void paging_map_identity(uint64_t base, size_t size, uint64_t flags) {
    for (uint64_t addr = base; addr < base + size; addr += PAGE_SIZE) {
        map_page(addr, addr, flags);
    }
}

void paging_map_kernel(uint64_t phys_base, uint64_t virt_base, size_t size, uint64_t flags) {
    for (size_t i = 0; i < size; i += PAGE_SIZE) {
        map_page(virt_base + i, phys_base + i, flags);
    }
}

void paging_unmap_page(uint64_t virt) {
    size_t pml4_index = (virt >> 39) & 0x1FF;
    size_t pdpt_index = (virt >> 30) & 0x1FF;
    size_t pd_index   = (virt >> 21) & 0x1FF;
    size_t pt_index   = (virt >> 12) & 0x1FF;

    if (!(pml4[pml4_index] & PAGE_PRESENT)) return;
    uint64_t* pdpt = (uint64_t*)(pml4[pml4_index] & ~0xFFFULL);

    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) return;
    uint64_t* pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFFULL);

    if (!(pd[pd_index] & PAGE_PRESENT)) return;
    uint64_t* pt = (uint64_t*)(pd[pd_index] & ~0xFFFULL);

    pt[pt_index] = 0;
    asm volatile ("invlpg (%0)" : : "r" (virt) : "memory");
}

uint64_t paging_get_phys(uint64_t virt) {
    size_t pml4_index = (virt >> 39) & 0x1FF;
    size_t pdpt_index = (virt >> 30) & 0x1FF;
    size_t pd_index   = (virt >> 21) & 0x1FF;
    size_t pt_index   = (virt >> 12) & 0x1FF;

    if (!(pml4[pml4_index] & PAGE_PRESENT)) return 0;
    uint64_t* pdpt = (uint64_t*)(pml4[pml4_index] & ~0xFFFULL);

    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) return 0;
    uint64_t* pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFFULL);

    if (!(pd[pd_index] & PAGE_PRESENT)) return 0;
    uint64_t* pt = (uint64_t*)(pd[pd_index] & ~0xFFFULL);

    if (!(pt[pt_index] & PAGE_PRESENT)) return 0;
    return pt[pt_index] & ~0xFFFULL;
}

void paging_init(uint64_t total_phys) {
    pml4 = alloc_page_table();
    if (!pml4) {
        extern void fb_write_text(uint64_t x, uint64_t y, const char* s, uint32_t fg);
        fb_write_text(0, 0, "PANIC: Failed to allocate PML4", 0xFF0000);
        for (;;) asm("hlt");
    }
    paging_map_identity(0, total_phys, PAGE_PRESENT | PAGE_RW);
    asm volatile("mov %0, %%cr3" :: "r"(pml4));
}
