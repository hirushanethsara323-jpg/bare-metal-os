#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdbool.h>

#define SERIAL_COM1_PORT 0x3F8

/* Serial Logging Levels */
typedef enum {
    KLOG_INFO,
    KLOG_WARN,
    KLOG_ERROR
} log_level_t;

/* Serial Driver Interface */
int serial_init(uint16_t port);
bool serial_received(uint16_t port);
char serial_read(uint16_t port);
bool serial_is_transmit_empty(uint16_t port);
void serial_putchar(uint16_t port, char c);
void serial_writestring(uint16_t port, const char* str);

/* High Level Kernel Debug Logger */
void klog(log_level_t level, const char* message);

#endif /* SERIAL_H */
