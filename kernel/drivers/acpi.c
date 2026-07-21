/**
 * Nothing OS - ACPI System Power Management & Table Discovery Engine
 * 
 * Scans BIOS memory space (0xE0000 - 0xFFFFF) for 'RSD PTR ' table signature,
 * parses ACPI Root System Description Tables, and manages ACPI soft poweroff.
 */

#include "../include/acpi.h"
#include "../include/io.h"
#include "../include/serial.h"

static acpi_rsdp_t* found_rsdp = NULL;

static int mem_cmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

/**
 * Scan BIOS main memory area (0xE0000 to 0xFFFFF) for "RSD PTR " signature.
 */
acpi_rsdp_t* acpi_find_rsdp(void) {
    if (found_rsdp != NULL) return found_rsdp;

    uint8_t* bios_mem = (uint8_t*)0x000E0000;
    for (uint32_t offset = 0; offset < 0x00020000; offset += 16) {
        if (mem_cmp(bios_mem + offset, ACPI_RSDP_SIGNATURE, 8) == 0) {
            found_rsdp = (acpi_rsdp_t*)(bios_mem + offset);
            return found_rsdp;
        }
    }
    return NULL;
}

bool acpi_init(void) {
    acpi_rsdp_t* rsdp = acpi_find_rsdp();
    if (rsdp != NULL) {
        klog(KLOG_INFO, "ACPI Root System Description Pointer (RSDP) Discovered in BIOS Memory.");
        return true;
    }
    klog(KLOG_WARN, "ACPI RSDP Table Signature Not Found in Standard BIOS Region.");
    return false;
}

/**
 * Trigger ACPI System Power Off (Soft Shutdown).
 */
void acpi_poweroff(void) {
    klog(KLOG_INFO, "Executing ACPI System Power Soft Off Signal...");
    /* Out command 0x2000 to QEMU / ACPI Power Management Port 0x604 */
    outb(0x604, 0x00);
    outb(0x604, 0x20);

    /* Fallback Port 0xB004 for Bochs/QEMU legacy shutdown */
    outb(0xB004, 0x00);
    outb(0xB004, 0x20);

    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}
