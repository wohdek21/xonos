#include "irq.h"
#include "idt.h"
#include "pic.h"
#include <stdint.h>
#include <stddef.h>
#include "time.h"


extern void fb_draw_rect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color);
extern void fb_write_text(uint64_t x, uint64_t y, const char* s, uint32_t fg);
extern void tick(void);
extern uint64_t counter;

extern void keyboard_handle_scancode(uint8_t scancode);

// Объявляем обработчики из irq.S
extern void irq_stub_0(void);
extern void irq_stub_1(void);
extern void irq_stub_2(void);
extern void irq_stub_3(void);
extern void irq_stub_4(void);
extern void irq_stub_5(void);
extern void irq_stub_6(void);
extern void irq_stub_7(void);
extern void irq_stub_8(void);
extern void irq_stub_9(void);
extern void irq_stub_10(void);
extern void irq_stub_11(void);
extern void irq_stub_12(void);
extern void irq_stub_13(void);
extern void irq_stub_14(void);
extern void irq_stub_15(void);

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void (*irq_stubs[16])(void) = {
    irq_stub_0, irq_stub_1, irq_stub_2, irq_stub_3,
    irq_stub_4, irq_stub_5, irq_stub_6, irq_stub_7,
    irq_stub_8, irq_stub_9, irq_stub_10, irq_stub_11,
    irq_stub_12, irq_stub_13, irq_stub_14, irq_stub_15,
};

void irq_init(void) {
    extern void irq_stub_0(void);
    idt_set_gate(32, irq_stub_0, 0x8E);
    for (int i = 0; i < 16; i++) {
        idt_set_gate(32 + i, irq_stubs[i], 0x8E); // 0x8E = present, ring 0, interrupt gate
    }
}


void irq_handler(uint64_t irq) {
    //draw_rect(0, 0, 5, 5, 0xFFFFFF00); // жёлтый пиксель слева вверху
    if (irq == 1) { // клавиатура
        keyboard_handle_scancode(inb(0x60));
    }

    if (irq == 0) {
        tick();
    }

    pic_send_eoi(irq);
}
