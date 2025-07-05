#include "keyboard.h"
#include "framebuffer.h"
#include "mem.h"
#include <string.h>
#include <stdint.h>

extern void print_prompt(void);
extern uint8_t code_buffer[64];

static const char scancode_set1[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',   0, '\\',
    'z','x','c','v','b','n','m',',','.','/',   0, '*',  0,  ' ',
};

static int cursor_x = 0;
static int cursor_y = 0;
static char input_line[256];
static size_t input_len = 0;

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void print_prompt(void) {
    fb_write_text(cursor_x, cursor_y, "kernel> ", 0x00FFFF);
    cursor_x += 8 * 8;
}

static void clear_screen() {
    fb_draw_rect(0, 0, 640, 480, 0x000000);
    cursor_x = 0;
    cursor_y = 0;
    print_prompt();
}

static void run_user_program(void) {
    fb_write_text(0, cursor_y += 16, "Running...", 0xFFFF00);
    ((void(*)())code_buffer)();
}

static void parse_command(const char* cmd) {
    if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    } else if (strcmp(cmd, "run") == 0) {
        run_user_program();
    } else {
        fb_write_text(0, cursor_y += 16, "Unknown command", 0xFF0000);
    }
    cursor_x = 0;
    cursor_y += 16;
    print_prompt();
}

void keyboard_handle_scancode(uint8_t scancode) {
    if (scancode & 0x80) return;

    char c = scancode_set1[scancode];
    if (c == 0) return;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 16;
        input_line[input_len] = '\0';
        parse_command(input_line);
        input_len = 0;
        return;
    }

    if (c == '\b') {
        if (input_len > 0) {
            input_len--;
            if (cursor_x >= 8) cursor_x -= 8;
            fb_draw_rect(cursor_x, cursor_y, 8, 16, 0x000000);
        }
        return;
    }

    if (input_len < sizeof(input_line) - 1) {
        input_line[input_len++] = c;
        char buf[2] = {c, 0};
        fb_write_text(cursor_x, cursor_y, buf, 0xFFFFFF);
        cursor_x += 8;
        if (cursor_x >= 640) {
            cursor_x = 0;
            cursor_y += 16;
        }
    }
}
