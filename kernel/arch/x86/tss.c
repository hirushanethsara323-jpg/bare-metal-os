/**
 * Nothing OS - Task State Segment (TSS) & User Mode Ring 3 Privilege Switcher
 * 
 * Implements x86 Hardware Task State Segment (TSS) for Ring 0 Kernel Stack
 * switching during hardware interrupts and software IRET Ring 3 User Mode transition.
 */

#include "../include/tss.h"
#include "../include/serial.h"

static tss_entry_t tss;

/**
 * Initialize TSS structure and load Task Register (LTR).
 */
void tss_init(uint32_t idx, uint16_t kss, uint32_t kesp) {
    (void)idx;
    /* Clear TSS structure */
    uint8_t* ptr = (uint8_t*)&tss;
    for (size_t i = 0; i < sizeof(tss_entry_t); i++) {
        ptr[i] = 0;
    }

    tss.ss0 = kss;      /* Kernel Data Segment Selector (0x10) */
    tss.esp0 = kesp;    /* Kernel Stack Pointer */

    tss.cs = 0x0B;      /* User Code Segment Selector with RPL=3 */
    tss.ss = 0x13;      /* User Data Segment Selector with RPL=3 */
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;

    tss.iomap_base = sizeof(tss_entry_t);

    klog(KLOG_INFO, "Task State Segment (TSS) Configured for Ring 3 Privilege Escalation.");
}

/**
 * Update Kernel Stack Pointer in TSS for CPU interrupt stack frames.
 */
void tss_set_stack(uint16_t kss, uint32_t kesp) {
    tss.ss0 = kss;
    tss.esp0 = kesp;
}

/**
 * Perform Ring 3 User Mode Transition via IRET Assembly Frame.
 */
void switch_to_user_mode(void) {
    klog(KLOG_INFO, "Executing IRET Software Frame Transition to User Mode (Ring 3)...");

    /* Assembly IRET Frame Construction:
     * Push User Data Segment (0x23)
     * Push User Stack Pointer (ESP)
     * Push EFLAGS (0x202 - Interrupt Enable Bit set)
     * Push User Code Segment (0x1B)
     * Push Target Instruction Pointer (EIP)
     */
    __asm__ volatile (
        "cli\n"
        "mov $0x23, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "pushl $0x23\n"
        "pushl %%esp\n"
        "pushf\n"
        "popl %%eax\n"
        "orl $0x200, %%eax\n"  /* Enable IF Flag */
        "pushl %%eax\n"
        "pushl $0x1B\n"
        "pushl $1f\n"
        "iret\n"
        "1:\n"
        : : : "memory", "eax"
    );
}
