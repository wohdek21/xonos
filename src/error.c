#include "error.h"

void kernel_panic(void) {
    
    for (;;) __asm__("hlt");
}

void idt_panic(void) {
    
}