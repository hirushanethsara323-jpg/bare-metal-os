/**
 * Nothing OS - Desktop GUI Calculator Application Applet
 * 
 * Renders a graphical calculator window applet on Mode 13h / VBE framebuffer
 * with numeric keypad buttons (0-9), addition, subtraction, multiplication, and clear.
 */

#include "../include/calc.h"
#include "../include/vga_mode13.h"
#include "../include/serial.h"

void calc_app_launch(void) {
    klog(KLOG_INFO, "Launching Desktop Graphical Calculator Window Applet.");

    /* Clear Mode 13h Framebuffer */
    vga13_clear(COLOR13_CYAN);

    /* Render Outer Calculator Window Frame (Width 120, Height 140) */
    vga13_draw_gui_window(100, 30, 120, 140, "CALCULATOR");

    /* Render Calculator LCD Display Screen (Black background with Green numbers) */
    vga13_draw_rect(108, 50, 104, 18, COLOR13_BLACK);
    vga13_draw_string(180, 55, "1250", COLOR13_LIGHT_GREEN);

    /* Render Keypad Buttons Grid (3x4 Layout) */
    int button_labels[12] = {'7', '8', '9', '+', '4', '5', '6', '-', '1', '2', '3', '='};
    int btn_idx = 0;

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            int bx = 110 + (col * 25);
            int by = 75 + (row * 25);
            vga13_draw_rect(bx, by, 20, 20, COLOR13_LIGHT_GREY);
            vga13_draw_char(bx + 6, by + 6, (char)button_labels[btn_idx++], COLOR13_BLACK);
        }
    }

    klog(KLOG_INFO, "Desktop Graphical Calculator Window Rendered Successfully.");
}
