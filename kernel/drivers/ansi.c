/**
 * Nothing OS - ANSI Escape Sequence Parser & VGA Color Attribute Translator
 * 
 * Parses terminal escape sequences (\033[31m, \033[32m, \033[0m) and translates
 * them dynamically into hardware VGA text mode color attribute bytes.
 */

#include "../include/ansi.h"

extern void terminal_setcolor(uint8_t color);
extern void terminal_putchar(char c);

void ansi_print(const char* str) {
    if (str == 0) return;

    while (*str) {
        if (*str == '\033' && *(str + 1) == '[') {
            str += 2; /* Skip ESC and [ */

            /* Parse ANSI SGR code */
            int code = 0;
            while (*str >= '0' && *str <= '9') {
                code = code * 10 + (*str - '0');
                str++;
            }

            /* Translate ANSI SGR to VGA Color Attributes */
            switch (code) {
                case 0:  terminal_setcolor(0x07); break; /* Reset (Light Grey) */
                case 30: terminal_setcolor(0x00); break; /* Black */
                case 31: terminal_setcolor(0x0C); break; /* Red */
                case 32: terminal_setcolor(0x0A); break; /* Green */
                case 33: terminal_setcolor(0x0E); break; /* Yellow */
                case 34: terminal_setcolor(0x09); break; /* Blue */
                case 35: terminal_setcolor(0x0D); break; /* Magenta */
                case 36: terminal_setcolor(0x0B); break; /* Cyan */
                case 37: terminal_setcolor(0x0F); break; /* White */
                default: break;
            }

            if (*str == 'm') str++; /* Skip trailing 'm' */
        } else {
            terminal_putchar(*str);
            str++;
        }
    }
}
