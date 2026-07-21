/**
 * Nothing OS - PS/2 Keyboard Driver
 * 
 * Implements PS/2 keyboard controller interface, scancode translation,
 * shift/caps state tracking, and character/line input functions.
 */

#include "../include/keyboard.h"
#include "../include/io.h"

/* Terminal output function prototype from main.c */
extern void terminal_putchar(char c);

/* Shift & Caps Lock State */
static bool shift_pressed = false;
static bool caps_lock = false;

/* Unshifted Scancode (Set 1) -> ASCII Map */
static const char scancode_ascii_map[128] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,  '*',   0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,
    0,    0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0, 0, 0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0
};

/* Shifted Scancode (Set 1) -> ASCII Map */
static const char scancode_ascii_shift_map[128] = {
    0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,  '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,  '*',   0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,
    0,    0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0, 0, 0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0
};

/**
 * Initialize PS/2 Keyboard Driver.
 */
void keyboard_init(void) {
    shift_pressed = false;
    caps_lock = false;
    
    /* Flush any existing data in the keyboard output buffer */
    while (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        inb(KEYBOARD_DATA_PORT);
    }
}

/**
 * Check if keyboard data is available in the output buffer.
 */
bool keyboard_has_input(void) {
    return (inb(KEYBOARD_STATUS_PORT) & 0x01) != 0;
}

/**
 * Read raw scancode byte from the PS/2 data port.
 */
uint8_t keyboard_read_scancode(void) {
    while (!keyboard_has_input()) {
        __asm__ volatile ("hlt");
    }
    return inb(KEYBOARD_DATA_PORT);
}

/**
 * Translate PS/2 Set 1 scancode to corresponding ASCII character.
 * Returns 0 for non-printable keys (e.g. shift release, ctrl, etc.).
 */
char keyboard_scancode_to_ascii(uint8_t scancode) {
    /* Handle Shift press/release */
    if (scancode == SCANCODE_LSHIFT_PRESS || scancode == SCANCODE_RSHIFT_PRESS) {
        shift_pressed = true;
        return 0;
    }
    if (scancode == SCANCODE_LSHIFT_RELEASE || scancode == SCANCODE_RSHIFT_RELEASE) {
        shift_pressed = false;
        return 0;
    }
    
    /* Handle Caps Lock toggle */
    if (scancode == SCANCODE_CAPS_LOCK) {
        caps_lock = !caps_lock;
        return 0;
    }

    /* Ignore key releases (bit 7 set) */
    if (scancode & 0x80) {
        return 0;
    }

    /* Range check */
    if (scancode >= 128) {
        return 0;
    }

    char ch = 0;
    if (shift_pressed) {
        ch = scancode_ascii_shift_map[scancode];
    } else {
        ch = scancode_ascii_map[scancode];
    }

    /* Apply Caps Lock logic for alphabetic characters */
    if (caps_lock && ch >= 'a' && ch <= 'z') {
        if (!shift_pressed) ch -= 32; /* Convert to uppercase */
    } else if (caps_lock && ch >= 'A' && ch <= 'Z') {
        if (shift_pressed) ch += 32;  /* Shift + CapsLock -> lowercase */
    }

    return ch;
}

/**
 * Read next valid ASCII character from keyboard (blocking poll).
 */
char keyboard_getc(void) {
    char ch = 0;
    while (ch == 0) {
        uint8_t scancode = keyboard_read_scancode();
        ch = keyboard_scancode_to_ascii(scancode);
    }
    return ch;
}

/**
 * Read a full line of input from the keyboard into buffer.
 * Supports Backspace and echo printing on terminal.
 */
void keyboard_gets(char* buffer, size_t max_len) {
    size_t idx = 0;
    
    while (idx < max_len - 1) {
        char ch = keyboard_getc();
        
        if (ch == '\n') {
            terminal_putchar('\n');
            break;
        } else if (ch == '\b') {
            if (idx > 0) {
                idx--;
                terminal_putchar('\b');
            }
        } else if (ch >= 32 && ch <= 126) { /* Printable ASCII characters */
            buffer[idx++] = ch;
            terminal_putchar(ch);
        }
    }
    
    buffer[idx] = '\0';
}
