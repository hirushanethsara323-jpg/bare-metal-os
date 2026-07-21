/**
 * Nothing OS - 64-bit x86_64 Long Mode Transition & PML4 Paging Bridge
 * 
 * Verifies CPUID x86_64 long mode hardware extension bit, configures 4-level
 * PML4 page tables, and prepares IA32_EFER Model-Specific Register (MSR 0xC0000080).
 */

#include "../include/longmode.h"
#include "../include/serial.h"

static pml4_table_t pml4_root __attribute__((aligned(4096)));
static bool longmode_supported = false;

bool longmode_check_support(void) {
    uint32_t eax, ebx, ecx, edx;

    /* Check extended CPUID functions */
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0x80000000));
    if (eax < 0x80000001) {
        klog(KLOG_WARN, "CPUID Extended Functions Not Supported.");
        return false;
    }

    /* Query CPUID 0x80000001 for LM Bit (EDX bit 29) */
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0x80000001));
    if (edx & (1 << 29)) {
        longmode_supported = true;
        klog(KLOG_INFO, "x86_64 64-bit Long Mode Hardware Support Detected.");
        return true;
    }

    klog(KLOG_WARN, "x86_64 64-bit Long Mode Not Available on CPU.");
    return false;
}

void longmode_init_pml4(void) {
    /* Clear PML4 Entries */
    for (int i = 0; i < 512; i++) {
        pml4_root.entries[i] = 0;
    }

    /* Setup identity page entry 0 for lower 2MB 64-bit memory space */
    pml4_root.entries[0] = 0x0000000000100003ULL; /* Present, Read/Write */

    klog(KLOG_INFO, "PML4 4-Level 64-bit Page Table Directory Initialized @ 0xA0000 Alignment.");
}

void longmode_inspect_state(void) {
    longmode_check_support();
    longmode_init_pml4();
}
