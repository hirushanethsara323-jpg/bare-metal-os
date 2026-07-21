/**
 * Nothing OS - Local APIC & Multi-Core Symmetric Multiprocessing (SMP) Driver
 * 
 * Interforces with x86 Local Advanced Programmable Interrupt Controller (APIC)
 * via Memory-Mapped I/O @ 0xFEE00000, reads CPU Core IDs, and enables APIC software vectors.
 */

#include "../../include/apic.h"
#include "../../include/serial.h"

static volatile uint32_t* apic_mmio = (volatile uint32_t*)APIC_BASE_MMIO;
static bool apic_enabled = false;

uint32_t apic_read_register(uint32_t reg) {
    return apic_mmio[reg / 4];
}

void apic_write_register(uint32_t reg, uint32_t value) {
    apic_mmio[reg / 4] = value;
}

uint8_t apic_get_core_id(void) {
    if (!apic_enabled) return 0;
    return (uint8_t)((apic_read_register(APIC_REG_ID) >> 24) & 0xFF);
}

void apic_eoi(void) {
    if (apic_enabled) {
        apic_write_register(APIC_REG_EOI, 0);
    }
}

bool apic_init(void) {
    /* Check CPUID for APIC support (EDX bit 9) */
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));

    if (!(edx & (1 << 9))) {
        klog(KLOG_WARN, "Local APIC Hardware Not Supported by Processor CPUID.");
        return false;
    }

    /* Enable Local APIC via Spurious Interrupt Vector Register (SVR - 0x00F0)
     * Set bit 8 to enable software APIC, assign vector 0xFF */
    apic_write_register(APIC_REG_SVR, 0x01FF);
    apic_enabled = true;

    klog(KLOG_INFO, "Local APIC Multiprocessor Core Controller Initialized @ MMIO 0xFEE00000.");
    return true;
}
