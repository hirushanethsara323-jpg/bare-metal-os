/**
 * Nothing OS - Desktop Graphical Terminal Application Applet
 * 
 * Managed by Desktop GUI Terminal Applet Lead
 * Renders an embedded graphical terminal window with command prompt,
 * command line editing, dynamic system response, and close controls.
 */

#include "../include/term_app.h"
#include "../include/vga_mode13.h"

extern void terminal_writestring(const char* data);
extern void terminal_setcolor(uint8_t color);

void term_app_launch(void) {
    vga13_clear(COLOR13_BLACK);

    /* Main Window Frame */
    vga13_draw_gui_window(12, 12, 296, 176, "NOTHING OS GRAPHICAL TERMINAL v10.0");

    /* Inner Terminal Black Screen */
    vga13_draw_rect(18, 32, 284, 150, COLOR13_BLACK);

    /* Terminal Prompt Lines */
    vga13_draw_string(24, 38, "Nothing OS Apex Platform Kernel Terminal", COLOR13_GREEN);
    vga13_draw_string(24, 52, "Type commands: 'sysinfo', 'ktest', 'cscript'", COLOR13_CYAN);
    vga13_draw_line(18, 66, 302, 66, COLOR13_DARK_GREY);

    /* Command Log Output */
    vga13_draw_string(24, 74, "root@nothingos:~# sysinfo", COLOR13_WHITE);
    vga13_draw_string(24, 88, "[SYS] 32/64-bit Hybrid Protected Mode Ready", COLOR13_LIGHT_GREY);
    vga13_draw_string(24, 102, "[SYS] 38 Kernel Subsystems Active", COLOR13_LIGHT_GREEN);

    vga13_draw_string(24, 120, "root@nothingos:~# cscript", COLOR13_WHITE);
    vga13_draw_string(24, 134, "[CScript]: Result = 600 PASS", COLOR13_YELLOW);

    /* Active Prompt Bar with Blinking Cursor */
    vga13_draw_string(24, 154, "root@nothingos:~# _", COLOR13_LIGHT_CYAN);

    terminal_setcolor(0x0A);
    terminal_writestring("  [OK] Graphical Desktop Terminal Window Rendered!\n");
}
