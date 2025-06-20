#pragma once
#include <stdint.h>

void irq_init(void);
void irq_handler(uint64_t irq);
