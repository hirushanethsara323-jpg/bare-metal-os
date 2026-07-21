/**
 * Nothing OS - Virtual Memory Paging Engine & Page Fault Handler
 * 
 * Sets up x86 32-bit Paging, Page Directory, Page Tables, Identity Mapping
 * for low kernel memory (0x0 to 4MB), CR3 loading, CR0 Bit 31 activation,
 * and CR2 Fault Address capture.
 */

#include "../include/paging.h"
#include "../include/idt.h"
#include "../include/serial.h"

static page_directory_t kernel_directory __attribute__((aligned(4096)));
static page_table_t identity_page_table __attribute__((aligned(4096)));

static uint32_t last_fault_address = 0;

/**
 * Handle Page Fault CPU Exception Vector (ISR 14).
 */
void page_fault_handler(uint32_t error_code) {
    /* Read linear fault address from CR2 register */
    uint32_t cr2;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
    last_fault_address = cr2;

    klog(KLOG_ERROR, "CRITICAL: Page Fault Exception Detected!");
    (void)error_code;
}

/**
 * Initialize 32-bit x86 Virtual Memory Paging.
 */
void paging_init(void) {
    /* Clear Page Directory and Identity Table */
    for (int i = 0; i < 1024; i++) {
        kernel_directory.tables[i] = 0 | PAGE_RW; /* Not Present */
        identity_page_table.pages[i] = 0;
    }

    /* Identity Map first 4 MB (0x0 to 0x400000) */
    for (uint32_t i = 0; i < 1024; i++) {
        uint32_t phys = i * PAGE_SIZE;
        identity_page_table.pages[i] = phys | PAGE_PRESENT | PAGE_RW;
    }

    /* Set first PDE entry to identity table */
    kernel_directory.tables[0] = ((uint32_t)&identity_page_table) | PAGE_PRESENT | PAGE_RW;

    /* Register Page Fault Vector Interrupt 14 in IDT */
    idt_set_gate(14, (uint32_t)page_fault_handler, 0x08, IDT_FLAG_PRESENT | IDT_FLAG_GATE_32);

    /* Load Page Directory Physical Address into CR3 */
    uint32_t pd_addr = (uint32_t)&kernel_directory;
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pd_addr));

    /* Enable Paging by setting Bit 31 (PG) in Control Register CR0 */
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; /* Set PG Bit */
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));

    klog(KLOG_INFO, "x86 Paging Enabled via CR0 Control Register.");
}

/**
 * Retrieve last linear fault address captured in CR2.
 */
uint32_t paging_get_fault_address(void) {
    return last_fault_address;
}
