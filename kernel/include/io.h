#ifndef IO_H
#define IO_H

#include <stdint.h>

/**
 * Read a byte from the specified I/O port.
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * Write a byte to the specified I/O port.
 */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * Wait for an I/O operation to complete (writes to port 0x80).
 */
static inline void io_wait(void) {
    outb(0x80, 0);
}

#endif /* IO_H */
