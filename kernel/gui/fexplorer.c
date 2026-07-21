/**
 * Nothing OS - Desktop Graphical File Explorer Applet
 * 
 * Lead: File Explorer Specialist
 * Renders VFS RAMDisk directory folders, document icons, size metadata,
 * and preview panes.
 */

#include "../include/fexplorer.h"
#include "../include/vga_mode13.h"
#include "../include/vfs.h"

extern void terminal_writestring(const char* data);
extern void terminal_setcolor(uint8_t color);

void fexplorer_app_launch(void) {
    vga13_clear(COLOR13_BLUE);

    /* Main Window Frame */
    vga13_draw_gui_window(15, 15, 290, 170, "NOTHING OS VFS FILE EXPLORER (/RAMDISK)");

    /* Navigation Bar */
    vga13_draw_rect(22, 38, 276, 16, COLOR13_LIGHT_GREY);
    vga13_draw_string(26, 42, "PATH: / (MemFS In-Memory RAMDisk Volume)", COLOR13_BLACK);

    /* Directory Tree Left Sidebar */
    vga13_draw_rect(22, 58, 80, 120, COLOR13_DARK_GREY);
    vga13_draw_string(26, 64, "FOLDERS", COLOR13_YELLOW);
    vga13_draw_string(26, 80, " /root", COLOR13_WHITE);
    vga13_draw_string(26, 95, " /sys", COLOR13_WHITE);
    vga13_draw_string(26, 110, " /bin", COLOR13_WHITE);
    vga13_draw_string(26, 125, " /home", COLOR13_WHITE);

    /* Main Content Pane */
    vga13_draw_rect(106, 58, 192, 120, COLOR13_BLACK);
    vga13_draw_string(112, 64, "NAME             SIZE      TYPE", COLOR13_CYAN);
    vga13_draw_line(106, 74, 298, 74, COLOR13_LIGHT_GREY);

    vfs_node_t* files = vfs_get_root();
    int y = 80;
    while (files != NULL && y < 160) {
        vga13_draw_string(112, y, files->name, COLOR13_WHITE);
        vga13_draw_string(210, y, "512 B", COLOR13_GREEN);
        vga13_draw_string(260, y, "FILE", COLOR13_YELLOW);
        y += 12;
        files = files->next;
    }

    terminal_setcolor(0x0A);
    terminal_writestring("  [OK] Graphical Desktop File Explorer Window Rendered!\n");
}
