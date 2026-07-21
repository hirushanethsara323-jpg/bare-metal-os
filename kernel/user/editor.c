/**
 * Nothing OS - Kernel Embedded Console Text Editor Utility
 * 
 * Provides an interactive text editor directly in kernel mode for creating,
 * editing, and saving text documents back into the Virtual File System (MemFS RAMDisk).
 */

#include "../include/editor.h"
#include "../include/vfs.h"
#include "../include/keyboard.h"
#include "../include/serial.h"

extern void terminal_init(void);
extern void terminal_setcolor(uint8_t color);
extern void terminal_writestring(const char* data);

void editor_open(const char* filename) {
    if (filename == 0) return;

    klog(KLOG_INFO, "Opening Kernel Interactive Console Text Editor.");

    terminal_setcolor(0x1F); /* White text on Blue background */
    terminal_init();

    terminal_setcolor(0x1F);
    terminal_writestring("  =================================================================\n");
    terminal_writestring("              NOTHING OS KERNEL TEXT EDITOR - EDITING: ");
    terminal_writestring(filename);
    terminal_writestring("\n  =================================================================\n\n");

    /* Read existing file contents from VFS if available */
    const char* existing_data = vfs_read_file(filename);
    if (existing_data != 0) {
        terminal_setcolor(0x1E);
        terminal_writestring("  [EXISTING FILE CONTENT]:\n  ");
        terminal_writestring(existing_data);
        terminal_writestring("\n\n");
    }

    terminal_setcolor(0x1A);
    terminal_writestring("  Type new line content below and press ENTER to save to MemFS RAMDisk:\n  > ");

    char edit_buf[256];
    terminal_setcolor(0x1F);
    keyboard_gets(edit_buf, sizeof(edit_buf));

    /* Save back to VFS RAMDisk */
    if (vfs_write_file(filename, edit_buf)) {
        terminal_setcolor(0x1A);
        terminal_writestring("\n  [SUCCESS] Changes written and saved to VFS RAMDisk file successfully!\n");
    } else {
        terminal_setcolor(0x1C);
        terminal_writestring("\n  [ERROR] Failed to save changes to VFS RAMDisk file!\n");
    }

    terminal_setcolor(0x1F);
    terminal_writestring("  Press any key to exit editor and return to Nothing OS Shell...\n");
    keyboard_getc();

    terminal_init();
}
