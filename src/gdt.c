#include "gdt.h"

static struct gdt_entry gdt[3];
static struct gdt_ptr gdt_descriptor;

static void gdt_set_entry(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[i].limit_low    = (limit & 0xFFFF);
    gdt[i].base_low     = (base & 0xFFFF);
    gdt[i].base_middle  = (base >> 16) & 0xFF;
    gdt[i].access       = access;
    gdt[i].granularity  = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[i].base_high    = (base >> 24) & 0xFF;
}

void gdt_init(void) {
    // GDT[0] — null
    gdt_set_entry(0, 0, 0, 0, 0);

    // GDT[1] — код (ring 0, exec, readable, present)
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xA0);

    // GDT[2] — данные (ring 0, writable, present)
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC0);

    // Указатель GDT
    gdt_descriptor.limit = sizeof(gdt) - 1;
    gdt_descriptor.base  = (uint64_t)&gdt;

    // Загружаем GDT и сегменты
    asm volatile ("lgdt %0" : : "m"(gdt_descriptor));
    asm volatile (
    "mov $0x10, %%ax\n"
    "mov %%ax, %%ds\n"
    "mov %%ax, %%es\n"
    "mov %%ax, %%ss\n"
    "mov %%ax, %%fs\n"
    "mov %%ax, %%gs\n"
    "pushq $0x08\n"
    "leaq 1f(%%rip), %%rax\n"
    "pushq %%rax\n"
    "lretq\n"
    "1:\n"
    :
    :
    : "rax", "memory"
);

}
