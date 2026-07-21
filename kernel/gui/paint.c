/**
 * Nothing OS - Desktop Graphical Canvas Paint Applet
 * 
 * Provides a 256-color Mode 13h pixel canvas drawing application with color palette
 * selection, brush tools, and canvas clear controls.
 */

#include "../include/paint.h"
#include "../include/vga_mode13.h"
#include "../include/serial.h"

void paint_app_launch(void) {
    klog(KLOG_INFO, "Launching Desktop Graphical Canvas Paint Applet.");

    /* Clear Framebuffer */
    vga13_clear(COLOR13_DARK_GREY);

    /* Main Paint Window Box */
    vga13_draw_gui_window(10, 10, 300, 180, "PAINT CANVAS Studio");

    /* White Drawing Canvas Area (x=20, y=30, w=220, h=150) */
    vga13_draw_rect(20, 32, 220, 150, COLOR13_WHITE);

    /* Side Color Palette Toolbox (x=248) */
    uint8_t palette_colors[6] = {COLOR13_RED, COLOR13_GREEN, COLOR13_BLUE, COLOR13_YELLOW, COLOR13_PINK, COLOR13_BLACK};
    for (int i = 0; i < 6; i++) {
        vga13_draw_rect(248, 35 + (i * 22), 20, 18, palette_colors[i]);
    }

    /* Sample Canvas Brush Strokes */
    vga13_draw_rect(50, 60, 40, 40, COLOR13_BLUE);
    vga13_draw_rect(110, 90, 60, 30, COLOR13_RED);

    klog(KLOG_INFO, "Desktop Paint Canvas Rendered.");
}
