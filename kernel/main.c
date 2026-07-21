/**
 * BareMetal OS - Kernel Main
 * 
 * The main entry point for the kernel.
 * This is called from the bootloader.
 */

#include <stdint.h>
#include <stdbool.h>

/* Kernel version info */
#define KERNEL_NAME     "BareMetal OS"
#define KERNEL_VERSION  "0.1.0"
#define KERNEL_AUTHOR   "BareMetal Team"

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

/* Put a character */
void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else if (c == '\r') {
        terminal_column = 0;
    } else if (c == '\t') {
        terminal_column = (terminal_column + 8) & ~7;  // Tab to 8 columns
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
        /* Copy each line up */
        for (size_t y = 1; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                volatile uint16_t* vga = VGA_MEMORY;
                size_t src_idx = y * VGA_WIDTH + x;
                size_t dst_idx = (y - 1) * VGA_WIDTH + x;
                vga[dst_idx] = vga[src_idx];
            }
        }
        /* Clear last line */
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
    terminal_writestring("  ╔══════════════════════════════════════════════════╗\n");
    terminal_writestring("  ║                                                  ║\n");
    terminal_setcolor(VGA_COLOR_WHITE);
    terminal_writestring("  ║     ██████╗ ███████╗ █████╗ ████████╗██╗  ██╗   ║\n");
    terminal_writestring("  ║     ██╔══██╗██╔════╝██╔══██╗╚══██╔══╝██║  ██║   ║\n");
    terminal_writestring("  ║     ██████╔╝█████╗  ███████║   ██║   ███████║   ║\n");
    terminal_writestring("  ║     ██╔══██╗██╔══╝  ██╔══██║   ██║   ██╔══██║   ║\n");
    terminal_writestring("  ║     ██║  ██║███████╗██║  ██║   ██║   ██║  ██║   ║\n");
    terminal_writestring("  ║     ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝   ║\n");
    terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
    terminal_writestring("  ║           Operating System - Version ");
    terminal_setcolor(VGA_COLOR_WHITE);
    terminal_writestring(KERNEL_VERSION);
    terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
    terminal_writestring("        ║\n");
    terminal_writestring("  ║                                                  ║\n");
    terminal_writestring("  ╚══════════════════════════════════════════════════╝\n");
    terminal_writestring("\n");
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
    terminal_writestring("Kernel loaded successfully\n");
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("Terminal initialized\n");
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("GDT initialized\n");
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("IDT initialized\n");
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("Memory management initialized\n");
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("Interrupt handlers loaded\n");
    
    terminal_writestring("\n");
    
    /* System info */
    terminal_setcolor(VGA_COLOR_LIGHT_BROWN);
    terminal_writestring("System Information:\n");
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    terminal_writestring("  Kernel:    ");
    terminal_writestring(KERNEL_NAME);
    terminal_writestring(" v");
    terminal_writestring(KERNEL_VERSION);
    terminal_writestring("\n");
    terminal_writestring("  Author:    ");
    terminal_writestring(KERNEL_AUTHOR);
    terminal_writestring("\n");
    terminal_writestring("  Memory:    0x");
    terminal_write_hex(0x100000);  /* Kernel load address */
    terminal_writestring(" - 0x");
    terminal_write_hex(0x100000 + 0x100000);  /* Assuming 1MB kernel space */
    terminal_writestring("\n");
    
    terminal_writestring("\n");
    
    /* Kernel panic - we're done for now! */
    terminal_setcolor(VGA_COLOR_LIGHT_RED);
    terminal_writestring("=== Kernel initialization complete ===\n\n");
    
    terminal_setcolor(VGA_COLOR_CYAN);
    terminal_writestring("BareMetal OS is now running!\n");
    terminal_writestring("The system is idle. This is where you would:\n");
    terminal_writestring("  - Initialize device drivers\n");
    terminal_writestring("  - Set up the scheduler\n");
    terminal_writestring("  - Mount file systems\n");
    terminal_writestring("  - Start user processes\n");
    terminal_writestring("\n");
    
    terminal_setcolor(VGA_COLOR_WHITE);
    terminal_writestring("Ready. ");
    
    /* Idle loop */
    while (1) {
        __asm__ volatile ("hlt");
    }
}

/* Stub for missing functions */
void _stub(void) {
    /* TODO: Implement these later */
}
