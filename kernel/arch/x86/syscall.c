/**
 * Nothing OS - System Call Engine (INT 0x80 Dispatcher)
 * 
 * Provides kernel software interrupt vector 0x80 dispatching for system
 * operations including IO writes, process control, uptime, and memory allocation.
 */

#include "../../include/syscall.h"
#include "../../include/idt.h"
#include "../../include/heap.h"
#include "../../include/serial.h"

extern void terminal_writestring(const char* data);

/**
 * Handle system calls dispatched via INT 0x80 vector.
 */
int32_t syscall_dispatch(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    (void)arg2;
    (void)arg3;
    switch (num) {
        case SYS_YIELD:
            /* CPU Yield stub */
            __asm__ volatile ("hlt");
            return 0;
            
        case SYS_WRITE:
            if (arg1 != 0) {
                terminal_writestring((const char*)arg1);
                return 0;
            }
            return -1;
            
        case SYS_GETPID:
            return 1; /* Initial kernel init process PID = 1 */
            
        case SYS_UPTIME:
            return (int32_t)timer_get_uptime_sec();
            
        case SYS_ALLOC:
            return (int32_t)(uintptr_t)kmalloc((size_t)arg1);
            
        case SYS_FREE:
            if (arg1 != 0) {
                kfree((void*)(uintptr_t)arg1);
                return 0;
            }
            return -1;
            
        default:
            klog(KLOG_WARN, "Unknown System Call Invoked via INT 0x80!");
            return -1;
    }
}

/**
 * Software interrupt invocation stub using inline assembly.
 */
int32_t syscall_invoke(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    return syscall_dispatch(num, arg1, arg2, arg3);
}

/**
 * Initialize Software System Call Vector in IDT.
 */
void syscall_init(void) {
    /* Set Gate 0x80 in IDT for User/Kernel Mode System Calls */
    idt_set_gate(SYSCALL_VECTOR, (uint32_t)syscall_dispatch, 0x08, IDT_FLAG_PRESENT | IDT_FLAG_RING3 | IDT_FLAG_GATE_32);
}
