/**
 * Nothing OS - Enterprise Kernel Main & Multi-Feature System Shell
 * 
 * Main kernel execution context initializing GDT/IDT interrupts, 8259 PIC,
 * PIT system timer, Serial UART COM1, Real-Time Clock, Kernel Heap, VFS, and VGA Graphics.
 */

#include <stdint.h>
#include <stdbool.h>
#include "include/io.h"
#include "include/keyboard.h"
#include "include/heap.h"
#include "include/idt.h"
#include "include/serial.h"
#include "include/rtc.h"
#include "include/vfs.h"
#include "include/vga_graphics.h"

/* Kernel Metadata */
#define KERNEL_NAME     "Nothing OS"
#define KERNEL_VERSION  "0.3.0"
#define KERNEL_AUTHOR   "Nothing OS Development Corporation & AI Crew"

/* Physical Memory Markers */
#define KERNEL_HEAP_BASE 0x00300000 /* 3 MB mark */

/* VGA Text Mode Setup */
#define VGA_MEMORY       (uint16_t*)0xB8000
#define VGA_WIDTH        80
#define VGA_HEIGHT       25

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

static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color;
static ui_theme_t current_theme = THEME_CYBERPUNK;

/* Function Prototypes */
void terminal_init(void);
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_write_hex(uint32_t num);
void terminal_write_int(int num);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
size_t strlen(const char* str);
void run_kernel_shell(void);

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

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void terminal_init(void) {
    terminal_color = vga_get_theme_color(current_theme, false);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    volatile uint16_t* vga = VGA_MEMORY;
    size_t index = y * VGA_WIDTH + x;
    vga[index] = (uint16_t)color << 8 | (uint8_t)c;
}

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
    
    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }
    
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

void terminal_writestring(const char* data) {
    while (*data) {
        terminal_putchar(*data);
        data++;
    }
}

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

void print_banner(void) {
    uint8_t title_col = vga_get_theme_color(current_theme, true);
    uint8_t body_col  = vga_get_theme_color(current_theme, false);

    terminal_setcolor(title_col);
    terminal_writestring("\n");
    terminal_writestring("  ╔═══════════════════════════════════════════════════════════════╗\n");
    terminal_writestring("  ║                                                               ║\n");
    terminal_setcolor(body_col);
    terminal_writestring("  ║  ███╗   ██╗ ██████╗ ████████╗██╗  ██╗██╗███╗   ██╗ ██████╗    ║\n");
    terminal_writestring("  ║  ████╗  ██║██╔═══██╗╚══██╔══╝██║  ██║██║████╗  ██║██╔════╝    ║\n");
    terminal_writestring("  ║  ██╔██╗ ██║██║   ██║   ██║   ███████║██║██╔██╗ ██║██║  ███╗   ║\n");
    terminal_writestring("  ║  ██║╚██╗██║██║   ██║   ██║   ██╔══██║██║██║╚██╗██║██║   ██║   ║\n");
    terminal_writestring("  ║  ██║ ╚████║╚██████╔╝   ██║   ██║  ██║██║██║ ╚████║╚██████╔╝   ║\n");
    terminal_writestring("  ║  ╚═╝  ╚═══╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝    ║\n");
    terminal_setcolor(title_col);
    terminal_writestring("  ║                                                               ║\n");
    terminal_writestring("  ║         Graphic & VFS Edition - System Release v");
    terminal_setcolor(body_col);
    terminal_writestring(KERNEL_VERSION);
    terminal_setcolor(title_col);
    terminal_writestring("       ║\n");
    terminal_writestring("  ║                                                               ║\n");
    terminal_writestring("  ╚═══════════════════════════════════════════════════════════════╝\n");
    terminal_writestring("\n");
}

/* Interactive System Shell Routine */
void run_kernel_shell(void) {
    char input_buf[128];
    uint8_t title_col = vga_get_theme_color(current_theme, true);
    uint8_t body_col  = vga_get_theme_color(current_theme, false);
    
    terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
    terminal_writestring("[OK] Interactive Nothing OS Shell v0.3.0 Ready.\n");
    terminal_writestring("MemFS VFS RAMDisk & Color Themes active. Type 'help' for commands.\n\n");
    
    while (1) {
        terminal_setcolor(title_col);
        terminal_writestring("NothingOS> ");
        terminal_setcolor(body_col);
        
        keyboard_gets(input_buf, sizeof(input_buf));
        
        if (strlen(input_buf) == 0) {
            continue;
        }
        
        if (strcmp(input_buf, "help") == 0 || strcmp(input_buf, "?") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Available System Commands:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  ls / dir               - List VFS files in RAMDisk\n");
            terminal_writestring("  cat <file>             - View contents of a file\n");
            terminal_writestring("  touch <file>           - Create new empty file\n");
            terminal_writestring("  write <file> <text>    - Write text content to a file\n");
            terminal_writestring("  theme <name>           - Switch UI Theme (cyberpunk/matrix/ocean/amber/default)\n");
            terminal_writestring("  window <title>         - Render GUI Window Box component\n");
            terminal_writestring("  progress <percent>     - Render GUI Progress Bar component\n");
            terminal_writestring("  uptime                 - Show system running time and timer ticks\n");
            terminal_writestring("  time / date            - Display Real-Time Clock (RTC) date/time\n");
            terminal_writestring("  heap / mem             - Show kernel dynamic heap statistics\n");
            terminal_writestring("  alloc                  - Run dynamic memory allocation test\n");
            terminal_writestring("  echo <text>            - Print input string back to console\n");
            terminal_writestring("  klog <msg>             - Send telemetry message to Serial COM1 Port\n");
            terminal_writestring("  sysinfo                - View kernel architecture & system specs\n");
            terminal_writestring("  agents                 - View corporate AI Development Board\n");
            terminal_writestring("  clear                  - Clear terminal screen\n");
            terminal_writestring("  reboot                 - Hardware reset system via keyboard controller\n");
        } else if (strcmp(input_buf, "version") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring(KERNEL_NAME);
            terminal_writestring(" v");
            terminal_writestring(KERNEL_VERSION);
            terminal_writestring(" (Graphic & VFS Edition)\nMaintainer: ");
            terminal_writestring(KERNEL_AUTHOR);
            terminal_writestring("\n");
        } else if (strcmp(input_buf, "ls") == 0 || strcmp(input_buf, "dir") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("In-Memory VFS Directory Listing (/):\n");
            terminal_setcolor(body_col);
            
            vfs_node_t* file = vfs_get_root();
            if (file == NULL) {
                terminal_writestring("  (Directory is empty)\n");
            } else {
                while (file != NULL) {
                    terminal_writestring("  - ");
                    terminal_writestring(file->name);
                    terminal_writestring(" (");
                    terminal_write_int(file->size);
                    terminal_writestring(" bytes)\n");
                    file = file->next;
                }
            }
        } else if (strncmp(input_buf, "cat ", 4) == 0) {
            const char* fname = input_buf + 4;
            const char* content = vfs_read_file(fname);
            if (content != NULL) {
                terminal_setcolor(title_col);
                terminal_writestring("=== Contents of ");
                terminal_writestring(fname);
                terminal_writestring(" ===\n");
                terminal_setcolor(body_col);
                terminal_writestring(content);
                terminal_writestring("\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("File not found: ");
                terminal_writestring(fname);
                terminal_writestring("\n");
            }
        } else if (strncmp(input_buf, "touch ", 6) == 0) {
            const char* fname = input_buf + 6;
            if (vfs_create_file(fname, "") != NULL) {
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("File created in VFS RAMDisk: ");
                terminal_writestring(fname);
                terminal_writestring("\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("Failed to create file!\n");
            }
        } else if (strncmp(input_buf, "write ", 6) == 0) {
            /* Parse filename and text content */
            char fname[32];
            size_t i = 0;
            const char* ptr = input_buf + 6;
            while (*ptr && *ptr != ' ' && i < 31) {
                fname[i++] = *ptr++;
            }
            fname[i] = '\0';
            
            if (*ptr == ' ') ptr++;
            
            if (vfs_write_file(fname, ptr)) {
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("Successfully written to VFS file: ");
                terminal_writestring(fname);
                terminal_writestring("\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("Failed to write file!\n");
            }
        } else if (strncmp(input_buf, "theme ", 6) == 0) {
            const char* tname = input_buf + 6;
            if (strcmp(tname, "cyberpunk") == 0) {
                current_theme = THEME_CYBERPUNK;
            } else if (strcmp(tname, "matrix") == 0) {
                current_theme = THEME_MATRIX;
            } else if (strcmp(tname, "ocean") == 0) {
                current_theme = THEME_OCEAN;
            } else if (strcmp(tname, "amber") == 0) {
                current_theme = THEME_AMBER;
            } else if (strcmp(tname, "default") == 0) {
                current_theme = THEME_DEFAULT;
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("Unknown theme! Choose: cyberpunk, matrix, ocean, amber, default\n");
                continue;
            }
            
            title_col = vga_get_theme_color(current_theme, true);
            body_col  = vga_get_theme_color(current_theme, false);
            terminal_init();
            print_banner();
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("Active Theme Updated: ");
            terminal_writestring(vga_get_theme_name(current_theme));
            terminal_writestring("\n\n");
        } else if (strncmp(input_buf, "window ", 7) == 0) {
            vga_draw_window(10, 5, 60, 10, title_col, input_buf + 7);
            terminal_row = 16;
            terminal_column = 0;
        } else if (strncmp(input_buf, "progress ", 9) == 0) {
            /* Convert string integer */
            int p = 0;
            const char* s = input_buf + 9;
            while (*s >= '0' && *s <= '9') {
                p = p * 10 + (*s - '0');
                s++;
            }
            vga_draw_progress_bar(15, terminal_row + 1, 50, p, title_col);
            terminal_row += 3;
            terminal_column = 0;
        } else if (strcmp(input_buf, "uptime") == 0) {
            uint32_t sec = timer_get_uptime_sec();
            uint32_t ticks = timer_get_ticks();
            terminal_setcolor(title_col);
            terminal_writestring("System Uptime: ");
            terminal_setcolor(body_col);
            terminal_write_int(sec);
            terminal_writestring(" seconds (");
            terminal_write_int(ticks);
            terminal_writestring(" hardware PIT ticks @ 100Hz)\n");
        } else if (strcmp(input_buf, "time") == 0 || strcmp(input_buf, "date") == 0) {
            rtc_time_t t;
            rtc_read_time(&t);
            terminal_setcolor(title_col);
            terminal_writestring("Real-Time Clock (CMOS RTC): ");
            terminal_setcolor(body_col);
            terminal_write_int(t.year);
            terminal_putchar('-');
            if (t.month < 10) terminal_putchar('0');
            terminal_write_int(t.month);
            terminal_putchar('-');
            if (t.day < 10) terminal_putchar('0');
            terminal_write_int(t.day);
            terminal_writestring(" ");
            if (t.hour < 10) terminal_putchar('0');
            terminal_write_int(t.hour);
            terminal_putchar(':');
            if (t.minute < 10) terminal_putchar('0');
            terminal_write_int(t.minute);
            terminal_putchar(':');
            if (t.second < 10) terminal_putchar('0');
            terminal_write_int(t.second);
            terminal_writestring(" UTC\n");
        } else if (strcmp(input_buf, "heap") == 0 || strcmp(input_buf, "mem") == 0) {
            heap_stats_t stats;
            heap_get_stats(&stats);
            
            terminal_setcolor(title_col);
            terminal_writestring("Kernel Heap Statistics:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Heap Address:    0x");
            terminal_write_hex(KERNEL_HEAP_BASE);
            terminal_writestring("\n  Capacity:        ");
            terminal_write_int(stats.total_size / 1024);
            terminal_writestring(" KB (");
            terminal_write_int(stats.total_size);
            terminal_writestring(" bytes)\n  Allocated:       ");
            terminal_write_int(stats.allocated_bytes);
            terminal_writestring(" bytes\n  Free:            ");
            terminal_write_int(stats.free_bytes);
            terminal_writestring(" bytes\n  Blocks Count:    ");
            terminal_write_int(stats.total_blocks);
            terminal_writestring(" (Free: ");
            terminal_write_int(stats.free_blocks);
            terminal_writestring(")\n");
        } else if (strcmp(input_buf, "alloc") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Running Heap Allocation Benchmark:\n");
            char* buf = (char*)kmalloc(1024);
            if (buf) {
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("  [OK] Allocated 1024 bytes @ Address: 0x");
                terminal_write_hex((uintptr_t)buf);
                terminal_writestring("\n");
                kfree(buf);
                terminal_writestring("  [OK] Memory freed and coalesced.\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("  [FAIL] kmalloc failed!\n");
            }
        } else if (strncmp(input_buf, "echo ", 5) == 0) {
            terminal_setcolor(body_col);
            terminal_writestring(input_buf + 5);
            terminal_writestring("\n");
        } else if (strncmp(input_buf, "klog ", 5) == 0) {
            klog(KLOG_INFO, input_buf + 5);
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("[COM1] Telemetry message transmitted via serial UART 16550.\n");
        } else if (strcmp(input_buf, "sysinfo") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("System Architecture Information:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Kernel:     Nothing OS v0.3.0 (Graphic & VFS Release)\n");
            terminal_writestring("  CPU Mode:   32-bit x86 Protected Mode (i386)\n");
            terminal_writestring("  IDT Table:  256 Gate Descriptors Configured\n");
            terminal_writestring("  PIC:        8259 Remapped (Master 0x20, Slave 0x28)\n");
            terminal_writestring("  PIT Clock:  Channel 0 Configured @ 100 Hz\n");
            terminal_writestring("  UART Com1:  COM1 Port 0x3F8 @ 38400 Baud\n");
            terminal_writestring("  Heap:       1 MB Boundary Tag Allocator @ 0x00300000\n");
            terminal_writestring("  VFS:        In-Memory RAMDisk File System Active\n");
            terminal_writestring("  Graphics:   VGA UI Theme Palette Engine Active\n");
        } else if (strcmp(input_buf, "agents") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Nothing OS Executive AI Board & Engineering Corporation:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  👑 CEO & Lead OS Architect:   Overall Vision, PRs & Architecture\n");
            terminal_writestring("  🧠 Core Assembly Architect:   GDT, IDT, PIC & CPU Interrupts\n");
            terminal_writestring("  💾 Memory Systems Specialist: Physical Memory & Kernel Heap\n");
            terminal_writestring("  ⌨️ Hardware Driver Lead:      PS/2 Controller & Serial UART\n");
            terminal_writestring("  ⏰ Clock & Peripherals Lead:  RTC CMOS Real-Time Clock & PIT\n");
            terminal_writestring("  🎨 UI/UX Graphics Engine Lead:VGA Themes, Window Boxes & Color Palette\n");
            terminal_writestring("  📁 VFS & Storage Specialist:  In-Memory RAMDisk File System\n");
            terminal_writestring("  🐚 Shell & Userspace Lead:    Interactive System Console\n");
            terminal_writestring("  ⚙️ DevOps & QA Engineer:     Toolchain, Makefile & GitHub CI\n");
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
            terminal_writestring("\nType 'help' for commands.\n");
        }
    }
}

/* Main Kernel Entry */
void _kernel_main(void) {
    terminal_init();
    print_banner();
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Nothing OS Kernel loaded in 32-bit Protected Mode\n");
    
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("VGA Console Engine & Color Palette Theme Manager active\n");
    
    /* Initialize Serial COM1 Debug Logger */
    serial_init(SERIAL_COM1_PORT);
    klog(KLOG_INFO, "Nothing OS Kernel Bootstrapped Successfully.");
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Serial UART COM1 Debug Interface initialized @ 0x3F8\n");
    
    /* Initialize IDT and Remap 8259 PIC */
    idt_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Interrupt Descriptor Table (256 Gates) & 8259 PIC Remapped\n");
    
    /* Initialize PIT System Timer @ 100Hz */
    pit_init(100);
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Programmable Interval Timer (PIT) initialized @ 100 Hz\n");
    
    /* Initialize Dynamic Heap Allocator */
    heap_init(KERNEL_HEAP_BASE, HEAP_INITIAL_SIZE);
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Kernel Heap Allocator (1 MB) initialized @ 0x00300000\n");
    
    /* Initialize Virtual File System (MemFS RAMDisk) */
    vfs_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("In-Memory Virtual File System (MemFS RAMDisk) initialized\n");

    /* Initialize PS/2 Keyboard Driver */
    keyboard_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("PS/2 Keyboard Input Driver initialized\n");
    
    /* Enable Hardware Interrupts */
    __asm__ volatile ("sti");
    klog(KLOG_INFO, "Hardware interrupts enabled via STI.");
    
    /* Run Interactive System Shell */
    run_kernel_shell();
}

void _stub(void) {
    /* Stub routine */
}
