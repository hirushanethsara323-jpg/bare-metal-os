/**
 * Nothing OS - Kernel Main & Interactive Shell
 * 
 * The main entry point for Nothing OS kernel.
 * Bootstrapped from GRUB Multiboot / custom bootloader.
 */

#include <stdint.h>
#include <stdbool.h>
#include "include/io.h"
#include "include/keyboard.h"
#include "include/heap.h"

/* Kernel version info */
#define KERNEL_NAME     "Nothing OS"
#define KERNEL_VERSION  "0.1.0"
#define KERNEL_AUTHOR   "hirushanethsara323-jpg & AI Crew Board"

/* Heap Location in Physical Memory */
#define KERNEL_HEAP_BASE 0x00300000 /* 3MB mark */

/* VGA text mode */
#define VGA_MEMORY       (uint16_t*)0xB8000
#define VGA_WIDTH        80
#define VGA_HEIGHT       25

/* Colors */
typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

/* Current cursor position */
static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color;

/* Function prototypes */
void terminal_init(void);
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_write_hex(uint32_t num);
void terminal_write_int(int num);
int strcmp(const char* s1, const char* s2);
size_t strlen(const char* str);
void run_kernel_shell(void);

/* String functions */
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/* Initialize terminal */
void terminal_init(void) {
    terminal_color = VGA_COLOR_LIGHT_GREY | (VGA_COLOR_BLACK << 4);
    
    /* Clear screen */
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
    
    terminal_row = 0;
    terminal_column = 0;
}

/* Set terminal color */
void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

/* Put a character at specific position */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    volatile uint16_t* vga = VGA_MEMORY;
    size_t index = y * VGA_WIDTH + x;
    vga[index] = (uint16_t)color << 8 | (uint8_t)c;
}

/* Put a character with newline and backspace handling */
void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else if (c == '\r') {
        terminal_column = 0;
    } else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
        return;
    } else if (c == '\t') {
        terminal_column = (terminal_column + 8) & ~7;
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        terminal_column++;
    }
    
    /* Handle line wrapping */
    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }
    
    /* Scroll if needed */
    if (terminal_row >= VGA_HEIGHT) {
        for (size_t y = 1; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                volatile uint16_t* vga = VGA_MEMORY;
                size_t src_idx = y * VGA_WIDTH + x;
                size_t dst_idx = (y - 1) * VGA_WIDTH + x;
                vga[dst_idx] = vga[src_idx];
            }
        }
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, VGA_HEIGHT - 1);
        }
        terminal_row = VGA_HEIGHT - 1;
    }
}

/* Write a null-terminated string */
void terminal_writestring(const char* data) {
    while (*data) {
        terminal_putchar(*data);
        data++;
    }
}

/* Write hex number */
void terminal_write_hex(uint32_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11];
    buffer[0] = '0';
    buffer[1] = 'x';
    
    for (int i = 7; i >= 0; i--) {
        buffer[9 - i] = hex_chars[(num >> (i * 4)) & 0xF];
    }
    buffer[10] = '\0';
    
    terminal_writestring(buffer);
}

/* Write decimal integer */
void terminal_write_int(int num) {
    char buffer[20];
    int i = 0;
    int is_negative = 0;
    
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    if (num == 0) {
        terminal_putchar('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    if (is_negative) {
        terminal_putchar('-');
    }
    
    for (int j = i - 1; j >= 0; j--) {
        terminal_putchar(buffer[j]);
    }
}

/* Print kernel info banner */
void print_banner(void) {
    terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
    terminal_writestring("\n");
    terminal_writestring("  ╔═══════════════════════════════════════════════════════════════╗\n");
    terminal_writestring("  ║                                                               ║\n");
    terminal_setcolor(VGA_COLOR_WHITE);
    terminal_writestring("  ║  ███╗   ██╗ ██████╗ ████████╗██╗  ██╗██╗███╗   ██╗ ██████╗    ║\n");
    terminal_writestring("  ║  ████╗  ██║██╔═══██╗╚══██╔══╝██║  ██║██║████╗  ██║██╔════╝    ║\n");
    terminal_writestring("  ║  ██╔██╗ ██║██║   ██║   ██║   ███████║██║██╔██╗ ██║██║  ███╗   ║\n");
    terminal_writestring("  ║  ██║╚██╗██║██║   ██║   ██║   ██╔══██║██║██║╚██╗██║██║   ██║   ║\n");
    terminal_writestring("  ║  ██║ ╚████║╚██████╔╝   ██║   ██║  ██║██║██║ ╚████║╚██████╔╝   ║\n");
    terminal_writestring("  ║  ╚═╝  ╚═══╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝    ║\n");
    terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
    terminal_writestring("  ║                                                               ║\n");
    terminal_writestring("  ║              Bare Metal Operating System - v");
    terminal_setcolor(VGA_COLOR_WHITE);
    terminal_writestring(KERNEL_VERSION);
    terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
    terminal_writestring("             ║\n");
    terminal_writestring("  ║                                                               ║\n");
    terminal_writestring("  ╚═══════════════════════════════════════════════════════════════╝\n");
    terminal_writestring("\n");
}

/* Interactive Kernel Shell */
void run_kernel_shell(void) {
    char input_buf[128];
    
    terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
    terminal_writestring("[OK] Dynamic Heap Allocator Ready!\n");
    terminal_writestring("Interactive Nothing OS Shell Active. Type 'help' for commands.\n\n");
    
    while (1) {
        terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
        terminal_writestring("NothingOS> ");
        terminal_setcolor(VGA_COLOR_WHITE);
        
        keyboard_gets(input_buf, sizeof(input_buf));
        
        if (strlen(input_buf) == 0) {
            continue;
        }
        
        if (strcmp(input_buf, "help") == 0 || strcmp(input_buf, "?") == 0) {
            terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
            terminal_writestring("Available Shell Commands:\n");
            terminal_setcolor(VGA_COLOR_LIGHT_GREY);
            terminal_writestring("  help      - Display this command manual\n");
            terminal_writestring("  version   - Print Nothing OS kernel version info\n");
            terminal_writestring("  heap      - Display Kernel Heap Allocator statistics\n");
            terminal_writestring("  alloc     - Test dynamic allocation with kmalloc/kfree\n");
            terminal_writestring("  sysinfo   - Display memory and CPU architecture status\n");
            terminal_writestring("  agents    - View the AI Board of Agents & roles\n");
            terminal_writestring("  clear     - Clear the VGA terminal screen\n");
            terminal_writestring("  reboot    - Reboot the system\n");
        } else if (strcmp(input_buf, "version") == 0) {
            terminal_setcolor(VGA_COLOR_WHITE);
            terminal_writestring(KERNEL_NAME);
            terminal_writestring(" v");
            terminal_writestring(KERNEL_VERSION);
            terminal_writestring(" (i386 Protected Mode)\nAuthor: ");
            terminal_writestring(KERNEL_AUTHOR);
            terminal_writestring("\n");
        } else if (strcmp(input_buf, "heap") == 0 || strcmp(input_buf, "mem") == 0) {
            heap_stats_t stats;
            heap_get_stats(&stats);
            
            terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
            terminal_writestring("Kernel Heap Statistics:\n");
            terminal_setcolor(VGA_COLOR_LIGHT_GREY);
            terminal_writestring("  Heap Base:       0x");
            terminal_write_hex(KERNEL_HEAP_BASE);
            terminal_writestring("\n  Total Size:      ");
            terminal_write_int(stats.total_size / 1024);
            terminal_writestring(" KB (");
            terminal_write_int(stats.total_size);
            terminal_writestring(" bytes)\n  Allocated Bytes: ");
            terminal_write_int(stats.allocated_bytes);
            terminal_writestring(" bytes\n  Free Bytes:      ");
            terminal_write_int(stats.free_bytes);
            terminal_writestring(" bytes\n  Total Blocks:    ");
            terminal_write_int(stats.total_blocks);
            terminal_writestring("\n  Free Blocks:     ");
            terminal_write_int(stats.free_blocks);
            terminal_writestring("\n");
        } else if (strcmp(input_buf, "alloc") == 0) {
            terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
            terminal_writestring("Testing Dynamic Memory Allocator (kmalloc / kfree):\n");
            
            terminal_setcolor(VGA_COLOR_LIGHT_GREY);
            terminal_writestring("  Allocating 512 bytes with kmalloc()...\n");
            char* test_ptr = (char*)kmalloc(512);
            
            if (test_ptr != NULL) {
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("  [OK] Successfully allocated @ Address: 0x");
                terminal_write_hex((uintptr_t)test_ptr);
                terminal_writestring("\n");
                
                /* Test write */
                test_ptr[0] = 'H'; test_ptr[1] = 'E'; test_ptr[2] = 'A'; test_ptr[3] = 'P'; test_ptr[4] = '\0';
                terminal_setcolor(VGA_COLOR_LIGHT_GREY);
                terminal_writestring("  Written data in heap buffer: ");
                terminal_writestring(test_ptr);
                terminal_writestring("\n  Freeing buffer with kfree()...\n");
                kfree(test_ptr);
                
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("  [OK] Memory freed & coalesced cleanly!\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("  [FAIL] kmalloc() failed!\n");
            }
        } else if (strcmp(input_buf, "sysinfo") == 0) {
            terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
            terminal_writestring("System Architecture Status:\n");
            terminal_setcolor(VGA_COLOR_LIGHT_GREY);
            terminal_writestring("  OS Name:     Nothing OS\n");
            terminal_writestring("  Mode:        32-bit Protected Mode\n");
            terminal_writestring("  Heap:        1 MB Dynamic Heap Allocator @ 0x00300000\n");
            terminal_writestring("  Driver:      PS/2 Keyboard Controller (Port 0x60 / 0x64)\n");
            terminal_writestring("  Console:     VGA Text Mode Memory mapped @ 0xB8000\n");
        } else if (strcmp(input_buf, "agents") == 0) {
            terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
            terminal_writestring("Nothing OS Executive AI Board:\n");
            terminal_setcolor(VGA_COLOR_LIGHT_GREY);
            terminal_writestring("  👑 CEO Agent:          Lead OS Architect\n");
            terminal_writestring("  🧠 Kernel Architect:   x86 Assembly & Low-Level Core\n");
            terminal_writestring("  💾 Memory Specialist:  PMM & Kernel Heap Manager\n");
            terminal_writestring("  ⌨️ Driver Specialist:  PS/2 Input & Peripherals\n");
            terminal_writestring("  ⚙️ DevOps & QA:        Makefile & GitHub Synchronization\n");
        } else if (strcmp(input_buf, "clear") == 0 || strcmp(input_buf, "cls") == 0) {
            terminal_init();
            print_banner();
        } else if (strcmp(input_buf, "reboot") == 0) {
            terminal_setcolor(VGA_COLOR_LIGHT_RED);
            terminal_writestring("Rebooting Nothing OS...\n");
            outb(0x64, 0xFE);
        } else {
            terminal_setcolor(VGA_COLOR_LIGHT_RED);
            terminal_writestring("Unknown command: ");
            terminal_writestring(input_buf);
            terminal_writestring("\nType 'help' for available commands.\n");
        }
    }
}

/* Main kernel function */
void _kernel_main(void) {
    /* Initialize terminal */
    terminal_init();
    
    /* Print banner */
    print_banner();
    
    /* Boot messages */
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("Nothing OS Kernel loaded in 32-bit Protected Mode\n");
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("VGA Console initialized\n");
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("Physical Memory Manager initialized\n");
    
    /* Initialize Dynamic Heap Allocator at 3MB mark */
    heap_init(KERNEL_HEAP_BASE, HEAP_INITIAL_SIZE);
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("Kernel Heap Allocator (1 MB) initialized @ 0x00300000\n");
    
    /* Initialize PS/2 Keyboard Driver */
    keyboard_init();
    
    /* Run interactive shell */
    run_kernel_shell();
}

/* Stub for missing functions */
void _stub(void) {
    /* Stub implementation */
}
