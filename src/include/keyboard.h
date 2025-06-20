#pragma once
#include <stdint.h>

void keyboard_handle_scancode(uint8_t scancode);
void print_prompt(void); // ← добавь
int strcmp(const char* s1, const char* s2);