/**
 * BareMetal OS - Boot Entry Point
 * 
 * Pure C implementation with inline assembly.
 * This replaces the need for external assembly.
 */

#include <stdint.h>

/* Multiboot magic numbers */
#define MULTIBOOT_HEADER_MAGIC    0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/* Multiboot flags */
#define MULTIBOOT_FLAG_PAGE_ALIGN  0x00000001
#define MULTIBOOT_FLAG_MEMORY_INFO 0x00000002
#define MULTIBOOT_FLAG_VIDEO_MODE  0x00000004

/* Structure passed by GRUB */
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_win_seg;
    uint16_t vbe_win_func;
    uint16_t vbe_total_mem;
};

/* Multiboot header - must be in first 8KB and aligned on 4-byte boundary */
__attribute__((section(".multiboot"), aligned(4)))
static uint32_t multiboot_header[] = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_FLAG_PAGE_ALIGN | MULTIBOOT_FLAG_MEMORY_INFO | MULTIBOOT_FLAG_VIDEO_MODE,
    -(MULTIBOOT_HEADER_MAGIC + 
      MULTIBOOT_FLAG_PAGE_ALIGN + 
      MULTIBOOT_FLAG_MEMORY_INFO + 
      MULTIBOOT_FLAG_VIDEO_MODE),
    0,  /* header_addr */
    0,  /* load_addr */
    0,  /* load_end_addr */
    0,  /* bss_end_addr */
    0   /* entry_addr */
};

/* Global Descriptor Table structure */
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* GDT entries */
static struct gdt_entry gdt[3];
static struct gdt_ptr gdtp;

/* Initialize GDT */
static void init_gdt(void) {
    /* Null descriptor */
    gdt[0].limit_low = 0;
    gdt[0].base_low = 0;
    gdt[0].base_mid = 0;
    gdt[0].access = 0;
    gdt[0].flags = 0;
    gdt[0].base_high = 0;
    
    /* Code segment */
    gdt[1].limit_low = 0xFFFF;
    gdt[1].base_low = 0;
    gdt[1].base_mid = 0;
    gdt[1].access = 0x9A;  /* Present, Ring 0, Code, Readable */
    gdt[1].flags = 0xCF;   /* 4KB, 32-bit */
    gdt[1].base_high = 0;
    
    /* Data segment */
    gdt[2].limit_low = 0xFFFF;
    gdt[2].base_low = 0;
    gdt[2].base_mid = 0;
    gdt[2].access = 0x92;  /* Present, Ring 0, Data, Writeable */
    gdt[2].flags = 0xCF;
    gdt[2].base_high = 0;
    
    /* Load GDT */
    gdtp.limit = sizeof(gdt) - 1;
    gdtp.base = (uint32_t)&gdt;
    
    /* Load GDTR */
    __asm__ volatile (
        "lgdt %0\n"
        "movw $0x10, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movw %%ax, %%ss\n"
        "jmp $0x08, $.next\n"
        ".next:\n"
        : : "m"(gdtp) : "ax", "memory"
    );
}

/* Kernel entry point - called from assembly stub */
void _kernel_entry(struct multiboot_info* mboot_ptr) {
    /* Initialize GDT first */
    init_gdt();
    
    /* Check multiboot magic */
    uint32_t magic = 0;
    __asm__ volatile ("movl %%eax, %0" : "=r"(magic));
    
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        /* Not loaded by multiboot-compliant bootloader */
        while (1) __asm__ volatile ("hlt");
    }
    
    /* Memory info */
    uint32_t mem_lower = mboot_ptr->mem_lower;
    uint32_t mem_upper = mboot_ptr->mem_upper;
    
    /* Call main kernel */
    extern void _kernel_main(void);
    _kernel_main();
    
    /* Should never return */
    while (1) __asm__ volatile ("hlt");
}

/* Assembly stub to call C function */
__asm__(
    ".text\n"
    ".global _start\n"
    "_start:\n"
    "   movl %esp, %ebp\n"
    "   pushl %ebx\n"          /* multiboot_info pointer */
    "   call _kernel_entry\n"
    "   jmp .\n"
);
