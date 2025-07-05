#include "idt.h"
#include "framebuffer.h"

#define IDT_SIZE 256

static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtr;

void idt_set_gate(int vec, void (*handler)(void), uint8_t flags) {
    uint64_t addr = (uint64_t)handler;

    idt[vec].offset_low    = addr & 0xFFFF;
    idt[vec].selector      = 0x08;  
    idt[vec].ist           = 0;     
    idt[vec].type_attr     = flags;
    idt[vec].offset_middle = (addr >> 16) & 0xFFFF;
    idt[vec].offset_high   = (addr >> 32) & 0xFFFFFFFF;
    idt[vec].zero          = 0;
}

extern void isr_stub_0(void);
extern void isr_stub_6(void);
extern void isr_stub_13(void);
extern void isr_stub_14(void);

extern void isr_stub_6(void);
extern void isr_stub_13(void);
extern void isr_stub_14(void);

void idt_init(void) {
    for (int i = 0; i < IDT_SIZE; i++) {
        idt_set_gate(i, isr_stub_0, 0x8E);
    }

    idt_set_gate(6, isr_stub_6, 0x8E);
    idt_set_gate(13, isr_stub_13, 0x8E);
    idt_set_gate(14, isr_stub_14, 0x8E);

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)&idt;

    asm volatile ("lidt %0" : : "m"(idtr));
}


void isr6_handler(void) {
    fb_draw_rect(0, 0, 640, 480, 0x000000);  // clear screen
    fb_write_text(0, 0, "EXCEPTION: Invalid Opcode (#UD)", 0xFF0000);
    for (;;) asm volatile("cli; hlt");
}

void isr13_handler(void) {
    fb_draw_rect(0, 0, 640, 480, 0x000000);
    fb_write_text(0, 0, "EXCEPTION: General Protection Fault (#GP)", 0xFF0000);
    for (;;) asm volatile("cli; hlt");
}

void isr14_handler(void) {
    fb_write_text(0, 100, "EXCEPTION: Page Fault (#PF)", 0xFF0000);
    for (;;) asm volatile("cli; hlt");
}

