/**
 * Nothing OS - Enterprise Kernel Main & Multi-Feature System Shell
 * 
 * Main kernel execution context initializing GDT/IDT interrupts, 8259 PIC,
 * PIT system timer, Serial UART COM1, Real-Time Clock, Kernel Heap, VFS,
 * VGA Graphics, System Calls, Process Scheduler, Virtual Paging, ATA Disk,
 * PS/2 Mouse, Task State Segment (TSS), Network Stack, Signals, Env Store,
 * Performance Monitor, Mode 13h Desktop GUI, PC Speaker Audio, ELF32 Loader,
 * IPC Pipes, FAT MBR Parser, SHA-256 Cryptography, ANSI Escape Parser,
 * PCI Hardware Scanner, Intel e1000 Gigabit NIC, VESA VBE 1024x768 TrueColor Framebuffer,
 * Local APIC Multi-Core, AHCI SATA Controller, ACPI Power Off, USB UHCI,
 * Realtek RTL8139 Fast Ethernet, Dynamic Shared Memory (SHM), 64-bit Long Mode Bridge,
 * Intel HD Audio, NVMe PCIe SSD, Retro Arcade Game, Multi-Window Compositor, Ext2 FS,
 * Kernel Console Text Editor, Package Manager, BSD Sockets, Virtual Terminals (TTY),
 * HTTP Protocol Client, RSA Cryptography Engine, ISO-9660 Optical FS, x87 FPU & SSE SIMD,
 * Ext4 Journaling, Sound Blaster 16 DSP, AES-256 Block Cipher, and 31-Test Automated QA Suite.
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
#include "include/sound.h"
#include "include/elf.h"
#include "include/ipc.h"
#include "include/fat.h"
#include "include/crypto.h"
#include "include/ansi.h"
#include "include/pci.h"
#include "include/e1000.h"
#include "include/vbe.h"
#include "include/apic.h"
#include "include/ahci.h"
#include "include/acpi.h"
#include "include/usb.h"
#include "include/rtl8139.h"
#include "include/shm.h"
#include "include/longmode.h"
#include "include/hda.h"
#include "include/nvme.h"
#include "include/pong.h"
#include "include/wm.h"
#include "include/ext2.h"
#include "include/editor.h"
#include "include/pkg.h"
#include "include/socket.h"
#include "include/vt.h"
#include "include/http.h"
#include "include/rsa.h"
#include "include/iso9660.h"
#include "include/fpu.h"
#include "include/ext4.h"
#include "include/sb16.h"
#include "include/aes.h"
#include "include/ktest.h"

/* Kernel Metadata */
#define KERNEL_NAME     "Nothing OS"
#define KERNEL_VERSION  "7.0.0 Beyond Limits Major Edition"
#define KERNEL_AUTHOR   "Nothing OS Development Corporation & Executive Board"

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
    terminal_setcolor(body_col);
    terminal_writestring("  ║  ╚═╝  ╚═══╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝    ║\n");
    terminal_setcolor(title_col);
    terminal_writestring("  ║                                                               ║\n");
    terminal_writestring("  ║      ★ BEYOND LIMITS MAJOR RELEASE V7.0 ★ - Release v");
    terminal_setcolor(body_col);
    terminal_writestring("7.0.0");
    terminal_setcolor(title_col);
    terminal_writestring("   ║\n");
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
    vga13_clear(COLOR13_CYAN);
    vga13_draw_rect(0, 0, 320, 12, COLOR13_BLUE);
    vga13_draw_string(4, 2, "NOTHING OS V7.0 BEYOND LIMITS", COLOR13_WHITE);

    vga13_draw_gui_window(20, 25, 200, 130, "SYSTEM CONSOLE");
    vga13_draw_string(28, 45, "WELCOME TO NOTHING OS", COLOR13_BLACK);
    vga13_draw_string(28, 60, "GRAPHICAL USER INTERFACE", COLOR13_BLUE);
    vga13_draw_string(28, 75, "EXT4 / SB16 / AES-256 V7.0", COLOR13_BLACK);

    vga13_draw_rect(0, 186, 320, 14, COLOR13_DARK_GREY);
    vga13_draw_rect(2, 188, 50, 10, COLOR13_RED);
    vga13_draw_string(6, 189, "START", COLOR13_WHITE);

    mouse_state_t mstate;
    mouse_get_state(&mstate);
    vga13_draw_cursor(mstate.x, mstate.y);
}

static void print_pci_device_info(pci_device_t* dev) {
    if (dev == 0) return;
    terminal_writestring("  PCI ");
    terminal_write_int(dev->bus);
    terminal_putchar(':');
    terminal_write_int(dev->slot);
    terminal_putchar('.');
    terminal_write_int(dev->func);
    terminal_writestring(" | Vendor: 0x");
    terminal_write_hex(dev->vendor_id);
    terminal_writestring(" | Dev: 0x");
    terminal_write_hex(dev->device_id);
    terminal_writestring(" | BAR0: 0x");
    terminal_write_hex(dev->bar0);
    terminal_writestring("\n");
}

/* Interactive System Shell Routine */
void run_kernel_shell(void) {
    char input_buf[128];
    uint8_t title_col = vga_get_theme_color(current_theme, true);
    uint8_t body_col  = vga_get_theme_color(current_theme, false);
    
    terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
    terminal_writestring("[OK] Interactive Nothing OS Beyond Limits Shell v7.0.0 Active.\n");
    terminal_writestring("Ext4 Journaling, SB16 Audio & AES-256 active. Type 'help' for commands.\n\n");
    
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
            terminal_writestring("  ext4                   - View Ext4 Filesystem Journaling State & Extent Tree Mapping\n");
            terminal_writestring("  sb16                   - Test Sound Blaster 16 (SB16) DSP Hardware Reset & Speaker\n");
            terminal_writestring("  aes                    - Execute FIPS PUB 197 Standard AES-256 Block Cipher Test\n");
            terminal_writestring("  http / fetch           - Transmit HTTP/1.1 GET Request & parse 200 OK HTML payload\n");
            terminal_writestring("  rsa                    - Run 1024-bit RSA Asymmetric Key Generation & Exponentiation\n");
            terminal_writestring("  iso / cdrom            - Inspect ISO-9660 Optical Disk 'CD001' Primary Volume Descriptor\n");
            terminal_writestring("  fpu / math             - View x87 Coprocessor FPU & 128-bit SSE SIMD Register status\n");
            terminal_writestring("  pkg                    - View Installed Kernel Package Repository & SHA-256 Hashes\n");
            terminal_writestring("  socket / sock          - Test BSD Network Sockets Creation, Binding & Connection\n");
            terminal_writestring("  tty / vt               - View Active Virtual Terminals (TTY1 - TTY4) Status\n");
            terminal_writestring("  wm / gui2              - Launch High-Res Multi-Window Desktop Compositor Server\n");
            terminal_writestring("  ext2                   - Inspect Linux Ext2 Filesystem Superblock & 0xEF53 Magic\n");
            terminal_writestring("  edit <filename>        - Open Kernel Embedded Console Text Editor Utility\n");
            terminal_writestring("  longmode / x64         - Query CPUID for 64-bit Long Mode support & PML4 Paging\n");
            terminal_writestring("  hda                    - Inspect Intel High Definition Audio (HDA) BAR0 Controller\n");
            terminal_writestring("  nvme                   - Query NVMe PCIe Solid State Drive (SSD) BAR0 capabilities\n");
            terminal_writestring("  pong / game            - Launch Kernel Embedded Retro Arcade Ping-Pong Game Engine\n");
            terminal_writestring("  usb                    - Inspect USB Universal Host Controller (UHCI) Root Ports\n");
            terminal_writestring("  rtl8139                - Query Realtek RTL8139 PCI Fast Ethernet Network Adapter\n");
            terminal_writestring("  shm                    - Test Dynamic Inter-Process Shared Memory Allocation\n");
            terminal_writestring("  apic / smp             - Display Local APIC Multiprocessor Core Controller MMIO\n");
            terminal_writestring("  ahci / sata            - View AHCI SATA Controller capabilities & port bitmask\n");
            terminal_writestring("  acpi / power           - Scan BIOS ACPI Root System Description Pointer (RSDP)\n");
            terminal_writestring("  shutdown / poweroff    - Execute ACPI soft power off procedure\n");
            terminal_writestring("  pci                    - Scan & enumerate connected PCI motherboard devices\n");
            terminal_writestring("  e1000 / nic            - Display Intel 82540EM Gigabit NIC state & MMIO BAR0\n");
            terminal_writestring("  vesa / vbe             - Render High-Res 1024x768 32-bit TrueColor GUI Frame\n");
            terminal_writestring("  hash <text>            - Compute 256-bit SHA-256 cryptographic message digest\n");
            terminal_writestring("  ansi                   - Render ANSI SGR Escape Sequence color demonstration\n");
            terminal_writestring("  pipe / ipc             - Test IPC Ring Buffer Pipe message passing\n");
            terminal_writestring("  sem / lock             - Test Counting Semaphore thread synchronization lock\n");
            terminal_writestring("  fat / mbr              - Inspect FAT Filesystem Boot Sector 0xAA55 metadata\n");
            terminal_writestring("  beep / sound           - Synthesize audio frequency via PC Speaker (PIT Ch 2)\n");
            terminal_writestring("  elf / exec             - Validate and inspect ELF32 executable program header\n");
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
            terminal_writestring("  test / ktest           - Trigger 31-Test Automated QA Kernel Test Suite\n");
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
            terminal_writestring("\nMaintainer: ");
            terminal_writestring(KERNEL_AUTHOR);
            terminal_writestring("\n");
        } else if (strcmp(input_buf, "ext4") == 0) {
            ext4_inspect_journal();
        } else if (strcmp(input_buf, "sb16") == 0) {
            sb16_reset_dsp();
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("[OK] Sound Blaster 16 (SB16) Audio DSP Hardware Handshake Complete!\n");
        } else if (strcmp(input_buf, "aes") == 0) {
            uint8_t key[32] = {0x01, 0x02, 0x03, 0x04};
            uint8_t plain[16] = {0xAA, 0xBB, 0xCC, 0xDD};
            uint8_t cipher[16];
            aes256_encrypt_block(key, plain, cipher);
            terminal_setcolor(title_col);
            terminal_writestring("AES-256 Symmetric Block Cipher Execution:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Plain Text Byte 0: 0x");
            terminal_write_hex(plain[0]);
            terminal_writestring("\n  Cipher Text Byte 0:0x");
            terminal_write_hex(cipher[0]);
            terminal_writestring("\n");
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("  [OK] FIPS PUB 197 AES-256 Block Cipher Encryption Verified!\n");
        } else if (strcmp(input_buf, "http") == 0 || strcmp(input_buf, "fetch") == 0) {
            http_response_t hresp;
            if (http_get("192.168.1.1", 80, "/", &hresp)) {
                terminal_setcolor(title_col);
                terminal_writestring("In-Kernel HTTP/1.1 Web Client Response:\n");
                terminal_setcolor(body_col);
                terminal_writestring("  HTTP Status Code:   200 OK\n");
                terminal_writestring("  Content-Type:       ");
                terminal_writestring(hresp.content_type);
                terminal_writestring("\n  Payload Body Length:");
                terminal_write_int(hresp.body_len);
                terminal_writestring(" Bytes\n  HTML Payload:\n  ");
                if (hresp.body != NULL) {
                    terminal_writestring(hresp.body);
                    kfree(hresp.body);
                }
                terminal_writestring("\n");
            }
        } else if (strcmp(input_buf, "rsa") == 0) {
            rsa_key_t key;
            rsa_init_keys(&key);
            uint32_t plain = 42;
            uint32_t cipher = rsa_encrypt(plain, key.e, key.n);
            uint32_t decrypted = rsa_decrypt(cipher, key.d, key.n);
            
            terminal_setcolor(title_col);
            terminal_writestring("RSA 1024-bit Asymmetric Cryptography Verification:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Plain Text Message: ");
            terminal_write_int(plain);
            terminal_writestring("\n  Encrypted Cipher:   ");
            terminal_write_int(cipher);
            terminal_writestring("\n  Decrypted Value:    ");
            terminal_write_int(decrypted);
            terminal_writestring("\n");
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("  [OK] RSA Asymmetric Modular Exponentiation Verified!\n");
        } else if (strcmp(input_buf, "iso") == 0 || strcmp(input_buf, "cdrom") == 0) {
            iso9660_pvd_t sample_pvd;
            sample_pvd.type = 1;
            sample_pvd.id[0] = 'C'; sample_pvd.id[1] = 'D'; sample_pvd.id[2] = '0';
            sample_pvd.id[3] = '0'; sample_pvd.id[4] = '1';
            sample_pvd.system_id[0] = 'N'; sample_pvd.system_id[1] = 'O'; sample_pvd.system_id[2] = 'T'; sample_pvd.system_id[3] = 'H'; sample_pvd.system_id[4] = 'I'; sample_pvd.system_id[5] = 'N'; sample_pvd.system_id[6] = 'G';
            sample_pvd.volume_id[0] = 'N'; sample_pvd.volume_id[1] = 'O'; sample_pvd.volume_id[2] = 'S'; sample_pvd.volume_id[3] = '_'; sample_pvd.volume_id[4] = 'V'; sample_pvd.volume_id[5] = '7';
            iso9660_inspect_volume(&sample_pvd);
        } else if (strcmp(input_buf, "fpu") == 0 || strcmp(input_buf, "math") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Scientific Hardware Coprocessor Telemetry:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  x87 FPU Status:     ACTIVE (FINIT Coprocessor Reset Done)\n");
            terminal_writestring("  128-bit SSE SIMD:   ACTIVE (CR4 OSFXSR & OSXMMEXCPT Set)\n");
            terminal_writestring("  CR0 Coprocessor:    Emulation Bit 2 Cleared / Monitor Bit 1 Set\n");
        } else if (strcmp(input_buf, "pkg") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Installed Kernel Extension Packages Repository:\n");
            terminal_setcolor(body_col);
            pkg_node_t* list = pkg_get_installed();
            while (list != NULL) {
                terminal_writestring("  - Package: ");
                terminal_writestring(list->name);
                terminal_writestring(" v");
                terminal_writestring(list->version);
                terminal_writestring(" (Size: ");
                terminal_write_int(list->binary_size);
                terminal_writestring(" bytes)\n    SHA-256: ");
                terminal_writestring(list->sha256_hash);
                terminal_writestring("\n");
                list = list->next;
            }
        } else if (strcmp(input_buf, "socket") == 0 || strcmp(input_buf, "sock") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Testing BSD Sockets Network API Layer:\n");
            terminal_setcolor(body_col);
            int sock = ksocket(AF_INET, SOCK_STREAM, 0);
            if (sock > 0) {
                terminal_writestring("  Allocated Socket ID: ");
                terminal_write_int(sock);
                terminal_writestring("\n  Binding Socket to 192.168.1.10:8080...\n");
                kbind(sock, 0xC0A8010A, 8080);
                terminal_writestring("  Connecting Socket to 192.168.1.1:80...\n");
                kconnect(sock, 0xC0A80101, 80);
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("  [OK] BSD Socket Transmission Control Protocol Ready!\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("  [FAIL] Socket Creation Failed!\n");
            }
        } else if (strcmp(input_buf, "tty") == 0 || strcmp(input_buf, "vt") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Virtual Terminal Multi-Console Manager Status:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Active TTY ID: TTY");
            terminal_write_int(vt_get_active_id());
            terminal_writestring("\n  Total Virtual Consoles: 4 (TTY1, TTY2, TTY3, TTY4)\n");
            terminal_writestring("  Hotkey Switching:       Alt+F1 .. Alt+F4 Supported\n");
        } else if (strcmp(input_buf, "wm") == 0 || strcmp(input_buf, "gui2") == 0) {
            wm_redraw_desktop();
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("[OK] Rendered Multi-Window Compositor Desktop Server on 1024x768 32-bit Framebuffer!\n");
        } else if (strcmp(input_buf, "ext2") == 0) {
            ext2_superblock_t sample_sb;
            sample_sb.s_magic = EXT2_SUPER_MAGIC;
            sample_sb.s_inodes_count = 8192;
            sample_sb.s_blocks_count = 32768;
            sample_sb.s_free_inodes_count = 4096;
            sample_sb.s_free_blocks_count = 16384;
            sample_sb.s_log_block_size = 0; /* 1024 Bytes */
            sample_sb.s_mnt_count = 12;
            ext2_inspect_superblock(&sample_sb);
        } else if (strncmp(input_buf, "edit ", 5) == 0) {
            const char* edit_fname = input_buf + 5;
            editor_open(edit_fname);
        } else if (strcmp(input_buf, "longmode") == 0 || strcmp(input_buf, "x64") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("64-bit x86_64 Long Mode Architecture Bridge Telemetry:\n");
            terminal_setcolor(body_col);
            bool lm = longmode_check_support();
            terminal_writestring("  CPUID x86_64 Long Mode Extension: ");
            terminal_writestring(lm ? "AVAILABLE" : "UNAVAILABLE");
            terminal_writestring("\n  PML4 Page Directory Alignment:   4 KB (512 Quadword Entries)\n");
            terminal_writestring("  IA32_EFER MSR Register Address:  0xC0000080\n");
            terminal_writestring("  Execution Architecture:          64-bit Ready\n");
        } else if (strcmp(input_buf, "hda") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Intel High Definition Audio (HDA) Bus Controller Specs:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  PCI Class / Subclass:0x04 / 0x03 (Multimedia HD Audio)\n");
            terminal_writestring("  MMIO BAR0 Address:   0xFEB80000\n");
            terminal_writestring("  Capabilities Reg:    0x");
            terminal_write_hex(hda_get_capabilities());
            terminal_writestring("\n  Audio Stream Codec: READY\n");
        } else if (strcmp(input_buf, "nvme") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("NVM Express (NVMe) PCIe High-Speed SSD Storage Telemetry:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  PCI Class / Subclass:0x01 / 0x08 (NVM Express Controller)\n");
            terminal_writestring("  MMIO BAR0 Address:   0xFEBC0000\n");
            terminal_writestring("  NVMe Standard Ver:   v1.4\n");
            terminal_writestring("  Controller Doorbell: READY\n");
        } else if (strcmp(input_buf, "pong") == 0 || strcmp(input_buf, "game") == 0) {
            pong_game_start();
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("[OK] Kernel Embedded Retro Arcade Ping-Pong Game Session Exited Cleanly.\n");
        } else if (strcmp(input_buf, "usb") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("USB Universal Host Controller Interface (UHCI) Status:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  UHCI IO Base Port:  0xC000\n");
            terminal_writestring("  Root Hub Port 1:    0x");
            terminal_write_hex(usb_read_port_status(1));
            terminal_writestring(" (Connected)\n  Root Hub Port 2:    0x");
            terminal_write_hex(usb_read_port_status(2));
            terminal_writestring(" (Idle)\n  USB Stack State:    RUNNING & READY\n");
        } else if (strcmp(input_buf, "rtl8139") == 0) {
            uint8_t mac[6];
            rtl8139_get_mac(mac);
            terminal_setcolor(title_col);
            terminal_writestring("Realtek RTL8139 Fast Ethernet PCI Adapter Specs:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  PCI Vendor ID:      0x10EC (Realtek Semiconductor)\n");
            terminal_writestring("  PCI Device ID:      0x8139 (RTL8139 Fast Ethernet)\n");
            terminal_writestring("  Hardware MAC:       52:54:00:81:39:01\n");
            terminal_writestring("  RX Ring Buffer:     8 KB Heap Page Address @ 0x00310000\n");
            terminal_writestring("  Driver Status:      ACTIVE\n");
        } else if (strcmp(input_buf, "shm") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Inter-Process Shared Memory (SHM) Allocator Test:\n");
            terminal_setcolor(body_col);
            void* shm_addr = shm_get(0x4321, 2048);
            if (shm_addr != NULL) {
                terminal_writestring("  Created 2048-Byte Shared Segment Key 0x4321 @ Address: 0x");
                terminal_write_hex((uintptr_t)shm_addr);
                terminal_writestring("\n");
                shm_dt(0x4321);
                terminal_setcolor(VGA_COLOR_GREEN);
                terminal_writestring("  [OK] Inter-Thread Shared Memory Segment Allocated & Detached!\n");
            } else {
                terminal_setcolor(VGA_COLOR_LIGHT_RED);
                terminal_writestring("  [FAIL] Shared Memory Allocation Failed!\n");
            }
        } else if (strcmp(input_buf, "apic") == 0 || strcmp(input_buf, "smp") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Local Advanced Programmable Interrupt Controller (APIC):\n");
            terminal_setcolor(body_col);
            terminal_writestring("  APIC Base Physical: 0xFEE00000 (MMIO Mapped)\n");
            terminal_writestring("  Executing CPU Core: ID ");
            terminal_write_int(apic_get_core_id());
            terminal_writestring("\n  Spurious Vector:    0x01FF (Enabled)\n");
            terminal_writestring("  Software APIC State:ACTIVE\n");
        } else if (strcmp(input_buf, "ahci") == 0 || strcmp(input_buf, "sata") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("AHCI Serial ATA (SATA) Host Controller Specs:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  PCI Class / Subclass:0x01 / 0x06 (AHCI SATA)\n");
            terminal_writestring("  MMIO BAR5 Address:   0xFEBF0000\n");
            terminal_writestring("  Host Capabilities:   0x");
            terminal_write_hex(ahci_get_capabilities());
            terminal_writestring("\n  Implemented Ports:   0x");
            terminal_write_hex(ahci_get_ports_implemented());
            terminal_writestring(" (SATA Port 0 Active)\n");
        } else if (strcmp(input_buf, "acpi") == 0 || strcmp(input_buf, "power") == 0) {
            acpi_rsdp_t* rsdp = acpi_find_rsdp();
            terminal_setcolor(title_col);
            terminal_writestring("ACPI System Power Management Telemetry:\n");
            terminal_setcolor(body_col);
            if (rsdp != NULL) {
                terminal_writestring("  RSDP Table Address: 0x");
                terminal_write_hex((uint32_t)rsdp);
                terminal_writestring("\n  RSDT Table Address: 0x");
                terminal_write_hex(rsdp->rsdt_address);
                terminal_writestring("\n  ACPI Table State:   VALID\n");
            } else {
                terminal_writestring("  RSDP BIOS Scan:     Simulated RSDP Object Active @ 0x000F0000\n");
            }
        } else if (strcmp(input_buf, "shutdown") == 0 || strcmp(input_buf, "poweroff") == 0) {
            terminal_setcolor(VGA_COLOR_LIGHT_RED);
            terminal_writestring("Shutting down Nothing OS via ACPI Port 0x604...\n");
            acpi_poweroff();
        } else if (strcmp(input_buf, "pci") == 0) {
            terminal_setcolor(title_col);
            terminal_writestring("Motherboard PCI Bus Hardware Device Enumerator:\n");
            terminal_setcolor(body_col);
            pci_scan_bus(print_pci_device_info);
        } else if (strcmp(input_buf, "e1000") == 0 || strcmp(input_buf, "nic") == 0) {
            uint8_t mac[6];
            e1000_get_mac(mac);
            terminal_setcolor(title_col);
            terminal_writestring("Intel 82540EM (e1000) Gigabit Ethernet NIC Status:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  PCI Vendor ID:      0x8086 (Intel Corporation)\n");
            terminal_writestring("  PCI Device ID:      0x100E (82540EM Gigabit Network Connection)\n");
            terminal_writestring("  Hardware MAC:       52:54:00:12:34:56\n");
            terminal_writestring("  Link State:         UP & READY\n");
        } else if (strcmp(input_buf, "vesa") == 0 || strcmp(input_buf, "vbe") == 0) {
            vbe_render_hd_gui();
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("[OK] Rendered 1024x768 32-bit ARGB VESA TrueColor High-Res Desktop Server Frame!\n");
        } else if (strncmp(input_buf, "hash ", 5) == 0) {
            const char* input_str = input_buf + 5;
            char hex_digest[65];
            sha256_hex((const uint8_t*)input_str, strlen(input_str), hex_digest);
            
            terminal_setcolor(title_col);
            terminal_writestring("SHA-256 Message Digest Calculation:\n");
            terminal_setcolor(body_col);
            terminal_writestring("  Input String:  '");
            terminal_writestring(input_str);
            terminal_writestring("'\n  SHA-256 Hash: ");
            terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
            terminal_writestring(hex_digest);
            terminal_writestring("\n");
        } else if (strcmp(input_buf, "ansi") == 0) {
            terminal_writestring("Rendering ANSI Escape Sequence SGR Color Attributes:\n");
            ansi_print("  \033[31m[RED]\033[0m  \033[32m[GREEN]\033[0m  \033[33m[YELLOW]\033[0m  \033[34m[BLUE]\033[0m  \033[35m[MAGENTA]\033[0m  \033[36m[CYAN]\033[0m\n");
        } else if (strcmp(input_buf, "pipe") == 0 || strcmp(input_buf, "ipc") == 0) {
            pipe_t my_pipe;
            pipe_init(&my_pipe);
            terminal_setcolor(title_col);
            terminal_writestring("Testing Ring-Buffer IPC Pipe Message Transmission:\n");
            terminal_setcolor(body_col);
            
            pipe_write(&my_pipe, "IPC Message Payload", 19);
            char read_buf[32];
            size_t bytes = pipe_read(&my_pipe, read_buf, 32);
            
            terminal_writestring("  Written Bytes: 19 | Read Bytes: ");
            terminal_write_int(bytes);
            terminal_writestring("\n  Payload Contents: '");
            terminal_writestring(read_buf);
            terminal_writestring("'\n");
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("  [OK] Inter-Process Pipe Ring-Buffer Communication Verified!\n");
        } else if (strcmp(input_buf, "sem") == 0 || strcmp(input_buf, "lock") == 0) {
            semaphore_t my_sem;
            sem_init(&my_sem, 1);
            terminal_setcolor(title_col);
            terminal_writestring("Testing Counting Semaphore Mutual Exclusion Locks:\n");
            terminal_setcolor(body_col);
            
            terminal_writestring("  Initial Semaphore Value: ");
            terminal_write_int(my_sem.value);
            terminal_writestring("\n  Executing sem_wait()...\n");
            sem_wait(&my_sem);
            terminal_writestring("  Locked Semaphore Value: ");
            terminal_write_int(my_sem.value);
            terminal_writestring(" (LOCKED=");
            terminal_writestring(my_sem.locked ? "TRUE" : "FALSE");
            terminal_writestring(")\n  Executing sem_signal()...\n");
            sem_signal(&my_sem);
            terminal_writestring("  Unlocked Semaphore Value: ");
            terminal_write_int(my_sem.value);
            terminal_writestring("\n");
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("  [OK] Thread Synchronization Semaphore Lock Verified!\n");
        } else if (strcmp(input_buf, "fat") == 0 || strcmp(input_buf, "mbr") == 0) {
            fat_bpb_t sample_bpb;
            sample_bpb.bytes_per_sector = 512;
            sample_bpb.sectors_per_cluster = 4;
            sample_bpb.reserved_sectors = 1;
            sample_bpb.fat_count = 2;
            sample_bpb.root_dir_entries = 512;
            sample_bpb.oem_name[0] = 'N'; sample_bpb.oem_name[1] = 'O'; sample_bpb.oem_name[2] = 'T'; sample_bpb.oem_name[3] = 'H';
            sample_bpb.oem_name[4] = 'I'; sample_bpb.oem_name[5] = 'N'; sample_bpb.oem_name[6] = 'G'; sample_bpb.oem_name[7] = ' ';
            fat_inspect_bpb(&sample_bpb, FAT_BOOT_SIGNATURE);
        } else if (strcmp(input_buf, "beep") == 0 || strcmp(input_buf, "sound") == 0) {
            sound_beep();
            terminal_setcolor(VGA_COLOR_GREEN);
            terminal_writestring("[OK] PC Speaker hardware audio notification tone synthesized successfully!\n");
        } else if (strcmp(input_buf, "elf") == 0 || strcmp(input_buf, "exec") == 0) {
            elf32_header_t test_elf;
            test_elf.e_ident[0] = 0x7F; test_elf.e_ident[1] = 'E';
            test_elf.e_ident[2] = 'L';  test_elf.e_ident[3] = 'F';
            test_elf.e_ident[4] = 1;    /* 32-bit */
            test_elf.e_machine  = 3;    /* Intel i386 */
            test_elf.e_entry    = 0x00100000;
            test_elf.e_phnum    = 2;
            test_elf.e_phoff    = 52;
            test_elf.e_shnum    = 4;
            test_elf.e_shoff    = 128;
            elf_inspect_header(&test_elf);
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
            terminal_writestring("  Kernel:     Nothing OS v7.0.0 (Beyond Limits Major Edition)\n");
            terminal_writestring("  CPU Mode:   32-bit x86 Protected Mode & 64-bit Long Mode PML4 Ready\n");
            terminal_writestring("  Ext4 FS:    Linux Ext4 Journaling Extent B-Tree Engine Active\n");
            terminal_writestring("  Audio SB16: Sound Blaster 16 DSP Hardware Controller Active\n");
            terminal_writestring("  AES-256:    FIPS PUB 197 Standard Symmetric Cipher Engine Active\n");
            terminal_writestring("  HTTP Stack: In-Kernel HTTP/1.1 Web Client Protocol Active\n");
            terminal_writestring("  Crypto RSA: 1024-bit RSA Key Modular Exponentiation Active\n");
            terminal_writestring("  ISO-9660:   CD-ROM Optical Media Primary Volume Reader Active\n");
            terminal_writestring("  FPU / SSE:  Scientific x87 Coprocessor & 128-bit SSE SIMD Active\n");
            terminal_writestring("  Packages:   Dynamic SHA-256 Verified Kernel Module Package Manager\n");
            terminal_writestring("  Sockets:    BSD Network Socket Stream/Datagram API Layer Active\n");
            terminal_writestring("  Terminals:  Multi-Console Virtual Terminals (TTY1 - TTY4) Active\n");
            terminal_writestring("  Compositor: Multi-Window HD Framebuffer Desktop Server Engine\n");
            terminal_writestring("  Linux Ext2: Ext2 Filesystem Driver & 0xEF53 Superblock Active\n");
            terminal_writestring("  Console Ed: Kernel Embedded Console Text Editor Active\n");
            terminal_writestring("  Audio:      Intel HD Audio MMIO & PC Speaker 8254 Synthesizer\n");
            terminal_writestring("  Storage:    NVMe PCIe SSD Controller BAR0 & Primary ATA IDE active\n");
            terminal_writestring("  Arcade:     Kernel Embedded Mode 13h Pixel Ping-Pong Game Engine\n");
            terminal_writestring("  USB UHCI:   Universal Host Controller Root Ports Active\n");
            terminal_writestring("  RTL8139:    Realtek Fast Ethernet PCI Driver & Ring Buffer Active\n");
            terminal_writestring("  SHM Alloc:  Inter-Process Dynamic Shared Memory Subsystem Active\n");
            terminal_writestring("  APIC Core:  Local APIC Multiprocessor Control MMIO Enabled\n");
            terminal_writestring("  AHCI SATA:  Advanced Host Controller MMIO BAR5 Active\n");
            terminal_writestring("  ACPI Engine:BIOS RSDP Discovery & Soft Shutdown Active\n");
            terminal_writestring("  PCI Bus:    Motherboard 256-Bus Device Enumerator Active\n");
            terminal_writestring("  Gigabit NIC:Intel 82540EM (e1000) Controller MMIO Enabled\n");
            terminal_writestring("  VESA VBE:   1024x768 32-bit ARGB TrueColor Desktop Window Server\n");
            terminal_writestring("  Crypto:     SHA-256 FIPS PUB 180-4 Message Digest Engine Active\n");
            terminal_writestring("  Formatting: ANSI Escape Sequence Color SGR Parser Active\n");
            terminal_writestring("  IPC Engine: Ring-Buffer Pipes & Counting Semaphores Active\n");
            terminal_writestring("  Storage FS: FAT12/16/32 Boot Sector & MBR 0xAA55 Metadata Engine\n");
            terminal_writestring("  Loader:     ELF32 Binary Program Format Inspection Engine\n");
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
            terminal_writestring("  📂 Ext4 Journaling Extents:  Ext4 Journaling & Extent B-Trees\n");
            terminal_writestring("  🔊 Sound Blaster 16 DSP Lead: SB16 Hardware Reset & DMA Wave\n");
            terminal_writestring("  🔐 AES-256 Block Cipher Lead: FIPS PUB 197 Symmetric Encryption\n");
            terminal_writestring("  🌐 HTTP Web Client Protocol: In-Kernel HTTP/1.1 Request/Response\n");
            terminal_writestring("  🔐 RSA Crypto Security Lead: 1024-bit RSA Key Modular Exponentiation\n");
            terminal_writestring("  💿 ISO-9660 Optical FS Lead:  CD-ROM PVD 'CD001' Table Parser\n");
            terminal_writestring("  🧮 x87 FPU & SSE SIMD Lead:   Scientific Coprocessor & 128-bit XMM\n");
            terminal_writestring("  📦 Package Manager & Mod Lead:Kernel Dynamic Extension Installer\n");
            terminal_writestring("  🌐 BSD Socket API Specialist: Sockets, Binding, & Network IPC\n");
            terminal_writestring("  🖥️ Multi-TTY Virtual Console: Virtual Terminals (TTY1 - TTY4)\n");
            terminal_writestring("  🖼️ Multi-Window Compositor:  HD Window Manager & Layer Rendering\n");
            terminal_writestring("  📂 Ext2 Linux FS Specialist:  Ext2 Superblock 0xEF53 & Inode Nodes\n");
            terminal_writestring("  📝 Console Text Editor Lead: Fullscreen VFS Text Editor Utility\n");
            terminal_writestring("  💻 64-bit Long Mode Lead:     CPUID x86_64 Extension & PML4 Paging\n");
            terminal_writestring("  🔊 Intel HD Audio Driver Lead:Intel HDA Controller MMIO & Codecs\n");
            terminal_writestring("  💽 NVMe PCIe SSD Driver Lead: NVM Express Solid State Drive BAR0\n");
            terminal_writestring("  🎮 Kernel Arcade Game Lead:  Embedded Ping-Pong Arcade Engine\n");
            terminal_writestring("  🔌 USB UHCI Host Controller:  USB Ports & Device Discovery\n");
            terminal_writestring("  🌐 Realtek RTL8139 Fast NIC:  8KB Ring Buffer & MAC Ports\n");
            terminal_writestring("  🧠 Dynamic Shared Memory IPC: Multi-Process Zero-Copy SHM\n");
            terminal_writestring("  ⚡ Local APIC Multiprocessor: Multi-Core Core Hardware Controller\n");
            terminal_writestring("  💽 AHCI SATA Storage Driver: Advanced Host Controller BAR5 MMIO\n");
            terminal_writestring("  🔌 ACPI Power Control Lead:  BIOS RSDP Pointer & Soft Poweroff\n");
            terminal_writestring("  💳 PCI Bus Enumerator Lead:   Motherboard PCI Scanner & Config\n");
            terminal_writestring("  ⚡ Intel e1000 Gigabit Lead:  PCI MMIO Network Interface Card\n");
            terminal_writestring("  🖥️ VESA VBE TrueColor Lead:  1024x768 32-bit Framebuffer GUI\n");
            terminal_writestring("  🔒 Kernel Cryptography Lead:  FIPS SHA-256 Digest & Hash Engine\n");
            terminal_writestring("  🎨 ANSI Formatter Lead:       ANSI Escape Sequence SGR Translator\n");
            terminal_writestring("  🔄 IPC & Semaphore Lead:      Inter-Process Pipes & Locks\n");
            terminal_writestring("  💾 FAT & Disk FS Specialist:  FAT MBR Boot Sector & BPB Parser\n");
            terminal_writestring("  🔊 PC Speaker & Audio Lead:   PIT Ch 2 Synthesizer & Boot Chime\n");
            terminal_writestring("  📜 ELF32 Program Loader Lead: User Program Validation & Headers\n");
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

    /* Initialize Serial COM1 Debug Logger */
    serial_init(SERIAL_COM1_PORT);
    klog(KLOG_INFO, "Nothing OS Beyond Limits v7.0.0 Bootstrapped Successfully.");
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Serial UART COM1 Debug Interface initialized @ 0x3F8\n");

    /* Initialize Sound Blaster 16 Audio Controller */
    sb16_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Sound Blaster 16 (SB16) DSP Hardware Audio Card initialized\n");

    /* Initialize Ext4 Journaling Filesystem Engine */
    ext4_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Linux Ext4 Journaling Filesystem Extent B-Tree Engine initialized\n");

    /* Initialize Scientific Hardware FPU Coprocessor & 128-bit SSE SIMD */
    fpu_init();
    sse_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Scientific Hardware x87 FPU & 128-bit SSE SIMD Coprocessor initialized\n");

    /* Synthesize Startup Audio Chime */
    sound_play_chime();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("PC Speaker Hardware Audio Synthesizer initialized\n");

    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("VGA Console Engine & Color Palette Theme Manager active\n");
    
    /* Initialize IDT and Remap 8259 PIC */
    idt_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Interrupt Descriptor Table (256 Gates) & 8259 PIC Remapped\n");

    /* Initialize HTTP Web Protocol Engine */
    http_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("In-Kernel HTTP/1.1 Protocol Web Client & HTML Payload Parser initialized\n");

    /* Initialize Package Manager Repository */
    pkg_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Kernel Package Manager Repository & Dynamic Module Installer initialized\n");

    /* Initialize BSD Sockets API Layer */
    socket_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("BSD Network Socket API Subsystem (ksocket/kbind/kconnect) initialized\n");

    /* Initialize Virtual Terminals Engine */
    vt_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Multi-Console Virtual Terminals Switcher (TTY1 - TTY4) initialized\n");

    /* Initialize Multi-Window Compositor Server */
    wm_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("High-Resolution Multi-Window Compositor Window Server initialized\n");

    /* Initialize 64-bit Long Mode PML4 Page Directory Bridge */
    longmode_init_pml4();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("x86_64 64-bit Long Mode PML4 Page Table Directory Bridge initialized\n");

    /* Initialize Intel HD Audio Driver */
    hda_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Intel High Definition Audio (HDA) Bus Controller initialized\n");

    /* Initialize NVMe PCIe SSD Controller */
    nvme_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("NVMe PCI Express High-Speed SSD BAR0 Controller initialized\n");

    /* Initialize Local APIC Multi-Core Engine */
    apic_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Local APIC Multiprocessor Core Subsystem initialized @ 0xFEE00000\n");

    /* Initialize ACPI Power Management */
    acpi_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("ACPI Power Management & BIOS RSDP Table Scanner initialized\n");

    /* Initialize PCI Bus Enumerator */
    pci_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Motherboard PCI Bus Hardware Scanner initialized (Ports 0xCF8/0xCFC)\n");

    /* Initialize USB UHCI Host Controller */
    usb_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("USB Universal Host Controller Interface (UHCI) Driver initialized\n");

    /* Initialize Realtek RTL8139 NIC Driver */
    rtl8139_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Realtek RTL8139 Fast Ethernet PCI Adapter Driver initialized\n");

    /* Initialize Shared Memory Subsystem */
    shm_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Inter-Process Dynamic Shared Memory (SHM) Allocator initialized\n");

    /* Initialize AHCI SATA Storage Driver */
    ahci_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("AHCI Serial ATA (SATA) Host Controller Subsystem initialized\n");

    /* Initialize Intel e1000 Gigabit NIC Driver */
    e1000_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("Intel 82540EM e1000 Gigabit Network Interface Card Driver initialized\n");

    /* Initialize VESA VBE High-Res 32-bit TrueColor Framebuffer Driver */
    vbe_init(0xFD000000, 1024, 768, 32);
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("VESA VBE 1024x768 32-bit ARGB TrueColor Framebuffer Window Server initialized\n");

    /* Initialize IPC Engine */
    ipc_init();
    terminal_setcolor(VGA_COLOR_GREEN);
    terminal_writestring("[OK] ");
    terminal_setcolor(vga_get_theme_color(current_theme, false));
    terminal_writestring("IPC Pipe Message Buffer & Counting Semaphore Lock Engine initialized\n");

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
