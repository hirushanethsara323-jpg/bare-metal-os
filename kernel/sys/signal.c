/**
 * Nothing OS - System Signal Engine & Emergency Kernel Panic Handler
 * 
 * Implements POSIX Process Signals (SIGINT, SIGKILL, SIGSEGV, SIGTERM)
 * and Kernel Panic Telemetry CPU Register Crash Dump.
 */

#include "../include/signal.h"
#include "../include/serial.h"
#include "../include/sched.h"

extern void terminal_setcolor(uint8_t color);
extern void terminal_writestring(const char* data);
extern void terminal_write_hex(uint32_t num);
extern void terminal_init(void);

static signal_handler_t signal_table[32];

static void default_sig_handler(int sig) {
    klog(KLOG_WARN, "Default Process Signal Delivered.");
    (void)sig;
}

void signal_init(void) {
    for (int i = 0; i < 32; i++) {
        signal_table[i] = default_sig_handler;
    }
    klog(KLOG_INFO, "POSIX System Signal Subsystem Initialized.");
}

bool signal_register(int sig, signal_handler_t handler) {
    if (sig < 1 || sig >= 32 || handler == 0) return false;
    signal_table[sig] = handler;
    return true;
}

bool signal_send(uint32_t pid, int sig) {
    if (sig < 1 || sig >= 32) return false;

    process_t* proc = process_get_list();
    while (proc != NULL) {
        if (proc->pid == pid) {
            if (sig == SIGKILL) {
                process_kill(pid);
                klog(KLOG_WARN, "Process Terminated via SIGKILL Signal.");
            } else {
                signal_table[sig](sig);
            }
            return true;
        }
        proc = proc->next;
    }
    return false;
}

/**
 * Emergency Kernel Panic Procedure.
 * Dumps CPU registers and renders Red Emergency Screen.
 */
void kernel_panic(const char* reason) {
    /* Capture CPU Control Registers */
    uint32_t cr0, cr2, cr3;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));

    /* Set VGA Red Panic Screen Attributes (White text on Red background: 0x4F) */
    terminal_setcolor(0x4F);
    terminal_init();
    terminal_setcolor(0x4F);

    terminal_writestring("\n");
    terminal_writestring("  =================================================================\n");
    terminal_writestring("             CRITICAL KERNEL PANIC - NOTHING OS STOP CODE           \n");
    terminal_writestring("  =================================================================\n\n");

    terminal_writestring("  REASON: ");
    terminal_writestring(reason ? reason : "Unknown Unhandled Fault");
    terminal_writestring("\n\n");

    terminal_writestring("  CPU CONTROL REGISTERS TELEMETRY:\n");
    terminal_writestring("    CR0 (Paging Control): 0x");
    terminal_write_hex(cr0);
    terminal_writestring("\n    CR2 (Page Fault Linear Address): 0x");
    terminal_write_hex(cr2);
    terminal_writestring("\n    CR3 (Page Directory Base Address): 0x");
    terminal_write_hex(cr3);
    terminal_writestring("\n\n");

    terminal_writestring("  The system has been halted to prevent corruption.\n");
    terminal_writestring("  Please reboot or inspect COM1 UART serial telemetry logs.\n");
    terminal_writestring("  =================================================================\n");

    klog(KLOG_ERROR, "CRITICAL: EMERGENCY KERNEL PANIC HALT TRIGGERED!");

    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}
