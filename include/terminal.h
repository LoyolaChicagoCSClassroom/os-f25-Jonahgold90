#pragma once
#include <stdint.h>
void terminal_init(uint8_t color);
int putc(int ch);
void terminal_set_color(uint8_t, color);
void terminal_clear(void);
