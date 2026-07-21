/**
 * Nothing OS - Multi-Virtual Terminal (TTY / VT) Console Switcher
 * 
 * Manages 4 independent hardware Virtual Terminals (TTY1 - TTY4), separate video frame
 * buffer states, cursor coordinates, and console hotkey switching.
 */

#include "../include/vt.h"
#include "../include/serial.h"

static vterminal_t terminals[MAX_VIRTUAL_TERMINALS];
static uint8_t active_vt_id = 1;

void vt_init(void) {
    for (uint8_t i = 0; i < MAX_VIRTUAL_TERMINALS; i++) {
        terminals[i].id = i + 1;
        terminals[i].cursor_x = 0;
        terminals[i].cursor_y = 0;
        terminals[i].color_attr = 0x07;
        terminals[i].active = (i == 0);

        /* Clear per-TTY video screen memory buffer */
        for (int cell = 0; cell < 80 * 25; cell++) {
            terminals[i].screen_buffer[cell] = 0x0720; /* Blank Space */
        }
    }

    active_vt_id = 1;
    klog(KLOG_INFO, "Multi-Console Virtual Terminals (TTY1 - TTY4) Initialized.");
}

void vt_switch(uint8_t id) {
    if (id < 1 || id > MAX_VIRTUAL_TERMINALS) return;

    for (uint8_t i = 0; i < MAX_VIRTUAL_TERMINALS; i++) {
        terminals[i].active = (terminals[i].id == id);
    }
    active_vt_id = id;

    klog(KLOG_INFO, "Switched Active Virtual Terminal TTY Console.");
}

uint8_t vt_get_active_id(void) {
    return active_vt_id;
}
