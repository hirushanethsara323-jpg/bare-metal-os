/**
 * Nothing OS - PS/2 Mouse Controller Driver
 * 
 * Configures PS/2 mouse streaming, 3-byte movement packet decoding,
 * screen cursor tracking (x, y), and button click detection.
 */

#include "../include/mouse.h"
#include "../include/io.h"
#include "../include/serial.h"

static mouse_state_t current_mouse = {40, 12, false, false, false};
static uint8_t mouse_cycle = 0;
static uint8_t mouse_byte[3];

static void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(0x64) & 1) == 1) return;
        }
    } else {
        while (timeout--) {
            if ((inb(0x64) & 2) == 0) return;
        }
    }
}

static void mouse_write(uint8_t write_byte) {
    mouse_wait(1);
    outb(0x64, 0xD4); /* Tell controller to send byte to auxiliary device (mouse) */
    mouse_wait(1);
    outb(0x60, write_byte);
}

static uint8_t mouse_read(void) {
    mouse_wait(0);
    return inb(0x60);
}

/**
 * Initialize PS/2 Auxiliary Mouse Device.
 */
void mouse_init(void) {
    uint8_t status;

    mouse_wait(1);
    outb(0x64, 0xA8); /* Enable auxiliary mouse device port */

    mouse_wait(1);
    outb(0x64, 0x20); /* Command: Read Compaq Status Byte */
    mouse_wait(0);
    status = (inb(0x60) | 2); /* Set Bit 1 (Enable IRQ12) */
    mouse_wait(1);
    outb(0x64, 0x60); /* Command: Write Compaq Status Byte */
    mouse_wait(1);
    outb(0x60, status);

    mouse_write(0xF6); /* Enable Default Mouse Settings */
    mouse_read();      /* Acknowledge */

    mouse_write(0xF4); /* Enable Data Reporting Packet Streaming */
    mouse_read();      /* Acknowledge */

    klog(KLOG_INFO, "PS/2 Auxiliary Mouse Controller Initialized.");
}

/**
 * Decode 3-byte PS/2 Mouse Data Packet.
 */
void mouse_handle_byte(uint8_t b) {
    switch (mouse_cycle) {
        case 0:
            if ((b & 0x08) == 0) break; /* Bit 3 must be 1 for valid byte 0 */
            mouse_byte[0] = b;
            mouse_cycle++;
            break;
        case 1:
            mouse_byte[1] = b;
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = b;
            mouse_cycle = 0;

            /* Parse Button Click Flags */
            current_mouse.left_button   = (mouse_byte[0] & 0x01) != 0;
            current_mouse.right_button  = (mouse_byte[0] & 0x02) != 0;
            current_mouse.middle_button = (mouse_byte[0] & 0x04) != 0;

            /* Calculate Deltas */
            int32_t dx = mouse_byte[1];
            int32_t dy = mouse_byte[2];

            if (mouse_byte[0] & 0x10) dx |= 0xFFFFFF00; /* Sign extend X */
            if (mouse_byte[0] & 0x20) dy |= 0xFFFFFF00; /* Sign extend Y */

            current_mouse.x += dx / 2;
            current_mouse.y -= dy / 2; /* Invert Y axis for screen space */

            /* Constrain Cursor to 80x25 VGA Grid Bounds */
            if (current_mouse.x < 0) current_mouse.x = 0;
            if (current_mouse.x >= 80) current_mouse.x = 79;
            if (current_mouse.y < 0) current_mouse.y = 0;
            if (current_mouse.y >= 25) current_mouse.y = 24;

            break;
    }
}

/**
 * Retrieve current mouse cursor coordinates and click state.
 */
void mouse_get_state(mouse_state_t* state) {
    if (state != 0) {
        *state = current_mouse;
    }
}
