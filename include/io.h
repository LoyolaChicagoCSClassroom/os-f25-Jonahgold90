#pragma once
#include <stdint.h>

//read a byte from IO port
uint8_t inb(uint16_t port);

//write 1 byte
void outb(uint16_t port, uint8_t val);

static inline void io_wait(void) {
    outb(0x80, 0);
}

