#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

/* PS/2 Keyboard Ports */
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

/* Special Key Scancodes (Set 1) */
#define SCANCODE_LSHIFT_PRESS   0x2A
#define SCANCODE_RSHIFT_PRESS   0x36
#define SCANCODE_LSHIFT_RELEASE 0xAA
#define SCANCODE_RSHIFT_RELEASE 0xB6
#define SCANCODE_CAPS_LOCK      0x3A
#define SCANCODE_ENTER          0x1C
#define SCANCODE_BACKSPACE      0x0E
#define SCANCODE_SPACE          0x39

/* Function Declarations */
void keyboard_init(void);
bool keyboard_has_input(void);
uint8_t keyboard_read_scancode(void);
char keyboard_scancode_to_ascii(uint8_t scancode);
char keyboard_getc(void);
void keyboard_gets(char* buffer, size_t max_len);

#endif /* KEYBOARD_H */
