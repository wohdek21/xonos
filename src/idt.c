#include "idt.h"

#define IDT_SIZE 256

static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtr;

void idt_set_gate(int vec, void (*handler)(void), uint8_t flags) {
    uint64_t addr = (uint64_t)handler;

    idt[vec].offset_low    = addr & 0xFFFF;
    idt[vec].selector      = 0x08;  // кодовый сегмент
    idt[vec].ist           = 0;     // 0 = без переключения стека
    idt[vec].type_attr     = flags; // 0x8E = present, ring 0, 64-bit interrupt
    idt[vec].offset_middle = (addr >> 16) & 0xFFFF;
    idt[vec].offset_high   = (addr >> 32) & 0xFFFFFFFF;
    idt[vec].zero          = 0;
}

extern void isr_stub_0(void);  // будет из asm

void idt_init(void) {
    for (int i = 0; i < IDT_SIZE; i++) {
        idt_set_gate(i, isr_stub_0, 0x8E); // временно всем один хендлер
    }

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)&idt;

    asm volatile ("lidt %0" : : "m"(idtr));
}
