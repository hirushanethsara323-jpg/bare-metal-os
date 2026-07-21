/**
 * Nothing OS - Scientific Hardware Floating Point Unit (x87 FPU & SSE SIMD) Driver
 * 
 * Configures Control Registers CR0 and CR4, initializes coprocessor registers
 * with FINIT instruction, and enables 128-bit XMM SSE SIMD instructions.
 */

#include "../include/fpu.h"
#include "../include/serial.h"

static bool fpu_enabled = false;

/**
 * Initialize x87 Coprocessor FPU hardware unit via CR0 and FINIT assembly.
 */
bool fpu_init(void) {
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));

    /* Clear Emulation Bit 2 (EM), Set Monitor Coprocessor Bit 1 (MP) */
    cr0 &= ~(1 << 2);
    cr0 |= (1 << 1);

    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));

    /* Execute FINIT Coprocessor Reset Assembly Instruction */
    __asm__ volatile ("finit");

    fpu_enabled = true;
    klog(KLOG_INFO, "Hardware x87 Floating Point Unit (FPU) Coprocessor Initialized.");
    return true;
}

/**
 * Enable 128-bit SSE SIMD instructions in Control Register CR4.
 */
bool sse_init(void) {
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));

    /* Set OSFXSR (Bit 9) and OSXMMEXCPT (Bit 10) */
    cr4 |= (1 << 9) | (1 << 10);

    __asm__ volatile ("mov %0, %%cr4" : : "r"(cr4));

    klog(KLOG_INFO, "128-bit SSE SIMD Hardware Registers Enabled via CR4 Register.");
    return true;
}

bool fpu_is_enabled(void) {
    return fpu_enabled;
}
