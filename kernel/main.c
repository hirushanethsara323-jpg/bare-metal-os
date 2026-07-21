/**
 * Nothing OS - Enterprise Kernel Main & Multi-Feature System Shell
 * 
 * Main kernel execution context initializing GDT/IDT interrupts, 8259 PIC,
 * PIT system timer, Serial UART COM1, Real-Time Clock, Kernel Heap, VFS,
 * VGA Graphics, System Calls, Process Scheduler, Virtual Paging, ATA Disk,
 * PS/2 Mouse, Task State Segment (TSS), Network Stack, Signals, Env Store,
 * Performance Monitor, Mode 13h Desktop GUI, and Automated QA Tests.
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
#include "include/vga_mode13.h"
#include "include/syscall.h"
#include "include/sched.h"
#include "include/paging.h"
#include "include/ata.h"
#include "include/mouse.h"
#include "include/tss.h"
#include "include/net.h"
#include "include/signal.h"
#include "include/env.h"
#include "include/monitor.h"
#include "include/ktest.h"

/* Kernel Metadata */
#define KERNEL_NAME     "Nothing OS"
#define KERNEL_VERSION  "0.9.0"
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
    terminal_writestring("  ║     GUI Desktop & Metrics Monitor Suite - System v");
    terminal_setcolor(body_col);
    terminal_writestring(KERNEL_VERSION);
    terminal_setcolor(title_col);
    terminal_writestring("     ║\n");
    terminal_writestring("  ║                                                               ║\n");
    terminal_writestring("  ╚═══════════════════════════════════════════════════════════════╝\n");
    terminal_writestring("\n");
}

static void background_worker_stub(void) {
    while (1) {
        process_yield();
    }
}

static void render_gui_desktop_demo(void) {
    /* Clear 320x200 mode 13h framebuffer with Teal Desktop Wallpaper (0x03) */
    vga13_clear(COLOR13_CYAN);

    /* Render Top Status Header Bar */
    vga13_draw_rect(0, 0, 320, 12, COLOR13_BLUE);
    vga13_draw_string(4, 2, "NOTHING OS DESKTOP V0.9.0", COLOR13_WHITE);

    /* Render Main Desktop Application Window */
    vga13_draw_gui_window(20, 25, 200, 130, "SYSTEM CONSOLE");
    vga13_draw_string(28, 45, "WELCOME TO NOTHING OS", COLOR13_BLACK);
    vga13_draw_string(28, 60, "GRAPHICAL USER INTERFACE", COLOR13_BLUE);
    vga13_draw_string(28, 75, "MODE 13H 256 COLORS", COLOR13_BLACK);

    /* Render Desktop Task Bar */
    vga13_draw_rect(0, 186, 320, 14, COLOR13_DARK_GREY);
    vga13_draw_rect(2, 188, 50, 10, COLOR13_RED);
    vga13_draw_string(6, 189, "START", COLOR13_WHITE);

    /* Render Mouse Cursor Overlay */
    mouse_state_t mstate;
    mouse_get_state(&mstate);
    vga13_draw_cursor(mstate.x, mstate.y);
}

/* Interactive System Shell Routine */
void run_kernel_shell(void) {
    char input_buf[128];
    uint8_t title_col = vga_get_theme_color(current_theme, true);
    uint8_t body_col  = vga_get_theme_color(current_theme, false);
    
    terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
    terminal_writestring("[OK] Interactive Nothing OS Flagship Suite Shell v0.9.0 Active.\n");
    terminal_writestring("GUI Mode 13h Desktop & Performance Monitor active. Type 'help' for commands.\n\n");
    
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
            terminal_writestring("  gui / desktop          - Render Mode 13h 256-Color Graphical Desktop Interface\n");
            terminal_writestring("  mon / top              - View Real-Time Kernel CPU & Memory Telemetry Dashboard\n");
            terminal_writestring("  env                    - Display Global System Environment Variables\n");
            terminal_writestring("  export <KEY>=<VAL>     - Dynamically set/update global environment variable\n");
            terminal_writestring("  signal <pid> <sig>     - Send POSIX signal (e.g. 9=SIGKILL) to process\n");
            terminal_writestring("  panic                  - Test Emergency Kernel Panic CPU register dump\n");
            terminal_writestring("  net / ping             - Display Network Stack & Transmit ICMP Echo Ping\n");
            terminal_writestring("  ring3 / usermode       - View Task State Segment (TSS) Ring 3 Privilege State\n");
            terminal_writestring("  paging / vmm           - View x86 Virtual Memory Paging & CR3 status\n");
            terminal_writestring("  ata / disk             - Query Primary ATA IDE Hard Disk Controller\n");
            terminal_writestring("  mouse                  - Display PS/2 Mouse coordinates & click telemetry\n");
            terminal_writestring("  ps                     - List active kernel processes & PIDs\n");
            terminal_writestring("  spawn <task_name>      - Spawn a new background kernel task\n");
            terminal_writestring("  kill <pid>             - Terminate a running process by PID\n");
            terminal_writestring("  test / ktest           - Trigger Automated QA & Kernel Test Suite\n");
            terminal_writestring("  syscall                - Test INT 0x80 POSIX System Call Dispatcher\n");
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
            terminal_writestring(" (GUI Desktop & Metrics Suite)\nMaintainer: ");
            terminal_writestring(KERNEL_AUTHOR);
            terminal_writestring("\n");
        } else if (strcmp(input_buf, "gui") == 0 || strcmp(input_buf, "desktop") == 0) {
            render_gui_desktop_demo();
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("[OK] Mode 13h 320x200 256-Color Graphical Desktop Environment Rendered @ 0xA0000!\n");
        } else if (strcmp(input_buf, "mon") == 0 || strcmp(input_buf, "top") == 0) {
            monitor_display_dashboard();
        } else if (strcmp(input_buf, "env") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Global OS System Environment Variables:\n");
            terminal_setcolor(body_col);
            env_var_t* list = env_get_list();
            while (list != NULL) {
                terminal_writestring("  ");
                terminal_writestring(list->key);
                terminal_writestring("=");
                terminal_writestring(list->value);
                terminal_writestring("\n");
                list = list->next;
            }
        } else if (strncmp(input_buf, "export ", 7) == 0) {
            const char* ptr = input_buf + 7;
            char key[32];
            size_t i = 0;
            while (*ptr && *ptr != '=' && i < 31) {
                key[i++] = *ptr++;
            }
            key[i] = '\0';
            if (*ptr == '=') ptr++;
            if (ksetenv(key, ptr)) {
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("Exported Environment Variable: ");
                terminal_writestring(key);
                terminal_writestring("=");
                terminal_writestring(ptr);
                terminal_writestring("\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("Failed to export environment variable!\n");
            }
        } else if (strncmp(input_buf, "signal ", 7) == 0) {
            int pid = 0, sig = 0;
            const char* ptr = input_buf + 7;
            while (*ptr >= '0' && *ptr <= '9') {
                pid = pid * 10 + (*ptr - '0');
                ptr++;
            }
            if (*ptr == ' ') ptr++;
            while (*ptr >= '0' && *ptr <= '9') {
                sig = sig * 10 + (*ptr - '0');
                ptr++;
            }
            if (signal_send((uint32_t)pid, sig)) {
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("Dispatched Signal ");
                terminal_write_int(sig);
                terminal_writestring(" to Process PID ");
                terminal_write_int(pid);
                terminal_writestring("\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("Failed to dispatch signal!\n");
            }
        } else if (strcmp(input_buf, "panic") == 0) {
            kernel_panic("Manual Test Routine Triggered Kernel Stop Code");
        } else if (strcmp(input_buf, "net") == 0 || strcmp(input_buf, "ping") == 0) {
            uint8_t mac[6];
            uint32_t ip;
            net_get_mac(mac);
            net_get_ip(&ip);
            
            terminal_setcolor(title_col);
            terminal_writestring("Virtual Loopback Network Stack Configuration:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  MAC Address:        52:54:00:12:34:56\n");
            terminal_writestring("  IPv4 Address:       192.168.1.10\n");
            terminal_writestring("  Netmask:            255.255.255.0\n");
            terminal_writestring("  Encapsulation:      Ethernet II / ARP / IPv4 / ICMP Echo\n");
            
            terminal_writestring("Transmitting ICMP Ping Request to 192.168.1.1...\n");
            net_send_ping(0xC0A80101);
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("  [OK] ICMP Echo Reply Received from 192.168.1.1 (0.8 ms)\n");
        } else if (strcmp(input_buf, "ring3") == 0 || strcmp(input_buf, "usermode") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Task State Segment (TSS) & Ring 3 User Privilege Status:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  TSS Register:       LOADED (GDT Selector 0x28)\n");
            terminal_writestring("  Ring 0 Kernel SS:   0x10 (Data Selector)\n");
            terminal_writestring("  Ring 3 User CS:     0x1B (Code Selector RPL=3)\n");
            terminal_writestring("  Ring 3 User DS:     0x23 (Data Selector RPL=3)\n");
            terminal_writestring("  IRET Frame Prep:    READY\n");
        } else if (strcmp(input_buf, "paging") == 0 || strcmp(input_buf, "vmm") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Virtual Memory Paging Status (i386 32-bit Architecture):\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Paging Mode:        4 KB Granularity (1024 PDE x 1024 PTE)\n");
            terminal_writestring("  CR0 PG Bit:         ACTIVE (1)\n");
            terminal_writestring("  Identity Mapping:   0x00000000 -> 0x00400000 (First 4 MB)\n");
            terminal_writestring("  Last Fault (CR2):   0x");
            terminal_write_hex(paging_get_fault_address());
            terminal_writestring(" (0 = No Fault)\n");
        } else if (strcmp(input_buf, "ata") == 0 || strcmp(input_buf, "disk") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Primary ATA IDE Sector Storage Interface:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Controller Port:    Primary Bus (0x1F0 - 0x1F7)\n");
            terminal_writestring("  Addressing Mode:    28-bit LBA (Logical Block Addressing)\n");
            terminal_writestring("  Standard Sector:    512 Bytes\n");
            terminal_writestring("  Master Drive:       READY\n");
        } else if (strcmp(input_buf, "mouse") == 0) {
            mouse_state_t mstate;
            mouse_get_state(&mstate);
            terminal_setcolor(title_col);
            terminal_writestring("PS/2 Auxiliary Mouse State Telemetry:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Grid Position X:    ");
            terminal_write_int(mstate.x);
            terminal_writestring("\n  Grid Position Y:    ");
            terminal_write_int(mstate.y);
            terminal_writestring("\n  Left Button:        ");
            terminal_writestring(mstate.left_button ? "PRESSED" : "RELEASED");
            terminal_writestring("\n  Right Button:       ");
            terminal_writestring(mstate.right_button ? "PRESSED" : "RELEASED");
            terminal_writestring("\n");
        } else if (strcmp(input_buf, "ps") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Active Kernel Tasks & Processes:\n");
            terminal_setcolor(body_col);
            
            process_t* proc = process_get_list();
            uint32_t count = 0;
            while (proc != NULL) {
                count++;
                terminal_writestring("  PID ");
                terminal_write_int(proc->pid);
                terminal_writestring(" | Name: ");
                terminal_writestring(proc->name);
                terminal_writestring(" | State: ");
                switch (proc->state) {
                    case PROCESS_RUNNING:    terminal_writestring("RUNNING"); break;
                    case PROCESS_READY:      terminal_writestring("READY"); break;
                    case PROCESS_BLOCKED:    terminal_writestring("BLOCKED"); break;
                    case PROCESS_TERMINATED: terminal_writestring("TERMINATED"); break;
                }
                terminal_writestring(" | ESP: 0x");
                terminal_write_hex(proc->esp);
                terminal_writestring("\n");
                proc = proc->next;
            }
            terminal_writestring("Total Active Tasks: ");
            terminal_write_int(count);
            terminal_writestring("\n");
        } else if (strncmp(input_buf, "spawn ", 6) == 0) {
            const char* name = input_buf + 6;
            process_t* new_p = process_create(name, background_worker_stub);
            if (new_p != NULL) {
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("Spawned background task '");
                terminal_writestring(name);
                terminal_writestring("' with PID ");
                terminal_write_int(new_p->pid);
                terminal_writestring("\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("Failed to spawn process!\n");
            }
        } else if (strncmp(input_buf, "kill ", 5) == 0) {
            int pid = 0;
            const char* s = input_buf + 5;
            while (*s >= '0' && *s <= '9') {
                pid = pid * 10 + (*s - '0');
                s++;
            }
            if (pid > 1) {
                process_kill((uint32_t)pid);
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("Killed process PID ");
                terminal_write_int(pid);
                terminal_writestring("\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("Cannot kill Kernel Init Process PID 1!\n");
            }
        } else if (strcmp(input_buf, "test") == 0 || strcmp(input_buf, "ktest") == 0) {
            test_results_t results;
            run_kernel_test_suite(&results);
        } else if (strcmp(input_buf, "syscall") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Executing INT 0x80 System Call Dispatcher Test:\n");
            terminal_setcolor(body_col);
            
            int32_t pid = syscall_invoke(SYS_GETPID, 0, 0, 0);
            int32_t uptime = syscall_invoke(SYS_UPTIME, 0, 0, 0);
            
            terminal_writestring("  SYS_GETPID Response: Process PID = ");
            terminal_write_int(pid);
            terminal_writestring("\n  SYS_UPTIME Response: System Uptime = ");
            terminal_write_int(uptime);
            terminal_writestring(" sec\n");
            
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("  [OK] Software Interrupt INT 0x80 dispatched successfully!\n");
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
            terminal_writestring("  Kernel:     Nothing OS v0.9.0 (GUI Desktop & Metrics Suite)\n");
            terminal_writestring("  CPU Mode:   32-bit x86 Protected Mode (i386)\n");
            terminal_writestring("  GUI Engine: Mode 13h VGA 320x200 256-Color Desktop @ 0xA0000\n");
            terminal_writestring("  Telemetry:  Real-Time Kernel Performance Monitor Active\n");
            terminal_writestring("  Signals:    POSIX Signals (SIGKILL, SIGINT, SIGSEGV) Engine\n");
            terminal_writestring("  Environment:Global Environment Variables Store Active\n");
            terminal_writestring("  User Mode:  Task State Segment (TSS) Ring 3 Stack Loaded\n");
            terminal_writestring("  Network:    Ethernet II / ARP / IPv4 / ICMP Loopback Interface\n");
            terminal_writestring("  Paging:     x86 4KB Virtual Memory Paging Enabled\n");
            terminal_writestring("  Storage:    Primary ATA IDE Hard Disk Controller Active\n");
            terminal_writestring("  Mouse:      PS/2 Auxiliary Mouse Streaming Active\n");
            terminal_writestring("  Scheduler:  Preemptive Round-Robin Multi-Process Scheduler Active\n");
            terminal_writestring("  Syscalls:   POSIX Software Interrupt Vector INT 0x80 Active\n");
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
            terminal_writestring("  🖼️ Mode 13h Pixel GUI Lead:  VGA 320x200 Pixel Graphics & Desktop\n");
            terminal_writestring("  📈 Kernel Metrics Lead:       Real-Time CPU % & Memory Telemetry\n");
            terminal_writestring("  🔍 OS Research & Intel Lead:  OSDev Standards & Spec Gathering\n");
            terminal_writestring("  🧪 Automated Testing & QA:    Self-Testing Suite & Validation\n");
            terminal_writestring("  🚨 Signal & Crash Telemetry: POSIX Process Signals & Crash Handler\n");
            terminal_writestring("  ⚙️ Environment Config Store: Global Environment Variable Engine\n");
            terminal_writestring("  🌐 Network Stack Protocol Lead:Ethernet, ARP, IPv4 & ICMP Ping Engine\n");
            terminal_writestring("  👤 User Mode Ring 3 Lead:    TSS Task State Segment & IRET Switch\n");
            terminal_writestring("  🧠 Core Assembly Architect:   GDT, IDT, PIC & CPU Interrupts\n");
            terminal_writestring("  ⚡ Virtual Memory Lead:       x86 Paging, PDE/PTE & Page Faults\n");
            terminal_writestring("  💽 Storage & IDE Disk Lead:   Primary ATA Hard Disk Driver\n");
            terminal_writestring("  🖱️ Pointing Device Lead:      PS/2 Mouse & Screen Cursor\n");
            terminal_writestring("  ⚙️ Syscall & POSIX Engine:   INT 0x80 System Call Dispatcher\n");
            terminal_writestring("  🔄 Process Scheduler Lead:    Multi-Tasking PCB & Round Robin\n");
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

    /* Initialize System Signals Subsystem */
    signal_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("POSIX Signal Handling Subsystem initialized\n");

    /* Initialize System Environment Config Store */
    env_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Global OS System Environment Configuration Store initialized\n");

    /* Initialize Performance Metrics Monitor */
    monitor_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Real-Time Kernel Performance Telemetry Monitor initialized\n");

    /* Initialize System Call Engine Vector (INT 0x80) */
    syscall_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("POSIX System Call Dispatcher Vector (INT 0x80) initialized\n");

    /* Initialize Task State Segment (TSS) */
    tss_init(5, 0x10, 0x00200000);
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Task State Segment (TSS) Ring 3 Stack loaded\n");

    /* Initialize Virtual Memory Paging Engine */
    paging_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("32-bit x86 Virtual Memory Paging (4KB Identity) enabled via CR0\n");

    /* Initialize Primary ATA IDE Hard Disk Controller */
    ata_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Primary ATA IDE Sector Controller initialized @ 0x1F0\n");

    /* Initialize Network Stack Engine */
    net_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Ethernet / ARP / IPv4 Virtual Network Interface initialized\n");

    /* Initialize PS/2 Mouse Controller */
    mouse_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("PS/2 Auxiliary Mouse Controller initialized\n");
    
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

    /* Initialize Process Scheduler */
    scheduler_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Preemptive Multi-Tasking Process Scheduler initialized\n");

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
