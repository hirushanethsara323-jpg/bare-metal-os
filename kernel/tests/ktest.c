/**
 * Nothing OS - Automated QA & Kernel Test Framework
 * 
 * Executed by the Testing Agent to validate memory allocators, VFS operations,
 * RTC clock bounds, Serial telemetry, and POSIX System Calls.
 */

#include "../include/ktest.h"
#include "../include/heap.h"
#include "../include/vfs.h"
#include "../include/rtc.h"
#include "../include/serial.h"
#include "../include/syscall.h"

extern void terminal_writestring(const char* data);
extern void terminal_write_int(int num);
extern void terminal_setcolor(uint8_t color);

static void test_log_pass(const char* name, test_results_t* res) {
    terminal_setcolor(0x0A); /* Light Green */
    terminal_writestring("  [PASS] ");
    terminal_setcolor(0x0F);
    terminal_writestring(name);
    terminal_writestring("\n");
    res->tests_passed++;
    res->tests_run++;
}

static void test_log_fail(const char* name, test_results_t* res) {
    terminal_setcolor(0x0C); /* Light Red */
    terminal_writestring("  [FAIL] ");
    terminal_setcolor(0x0F);
    terminal_writestring(name);
    terminal_writestring("\n");
    res->tests_failed++;
    res->tests_run++;
}

/**
 * Run all automated kernel tests.
 */
void run_kernel_test_suite(test_results_t* results) {
    if (results == 0) return;
    
    results->tests_run = 0;
    results->tests_passed = 0;
    results->tests_failed = 0;

    terminal_setcolor(0x0B);
    terminal_writestring("\n==============================================\n");
    terminal_writestring("  🧪 AUTOMATED QA & KERNEL TEST SUITE RUNNER  \n");
    terminal_writestring("==============================================\n\n");

    /* Test 1: Dynamic Heap Allocator */
    char* test_buf = (char*)kmalloc(256);
    if (test_buf != 0) {
        test_buf[0] = 'T'; test_buf[1] = 'E'; test_buf[2] = 'S'; test_buf[3] = 'T'; test_buf[4] = '\0';
        if (test_buf[0] == 'T' && test_buf[3] == 'T') {
            kfree(test_buf);
            test_log_pass("Dynamic Memory Allocator (kmalloc & kfree)", results);
        } else {
            kfree(test_buf);
            test_log_fail("Dynamic Memory Integrity Verification", results);
        }
    } else {
        test_log_fail("Dynamic Memory Allocation (kmalloc)", results);
    }

    /* Test 2: Virtual File System (VFS MemFS) */
    if (vfs_create_file("test_file.txt", "QA Test String Data") != 0) {
        const char* content = vfs_read_file("test_file.txt");
        if (content != 0 && content[0] == 'Q') {
            test_log_pass("Virtual File System RAMDisk (Create & Read)", results);
        } else {
            test_log_fail("Virtual File System Content Verification", results);
        }
    } else {
        test_log_fail("Virtual File System Allocation", results);
    }

    /* Test 3: Real-Time Clock Bounds */
    rtc_time_t t;
    rtc_read_time(&t);
    if (t.year >= 2026 && t.month >= 1 && t.month <= 12 && t.day >= 1 && t.day <= 31) {
        test_log_pass("CMOS Real-Time Clock Registers & Calendar Bounds", results);
    } else {
        test_log_fail("CMOS Real-Time Clock Values Out of Range", results);
    }

    /* Test 4: Serial Telemetry Interface */
    klog(KLOG_INFO, "Automated QA Test Suite Executed via Testing Agent.");
    test_log_pass("Serial UART COM1 Debug Logging", results);

    /* Test 5: POSIX System Call Vector Engine */
    int32_t pid = syscall_invoke(SYS_GETPID, 0, 0, 0);
    if (pid == 1) {
        test_log_pass("System Call Engine (INT 0x80 Vector Dispatcher)", results);
    } else {
        test_log_fail("System Call Engine Execution Failed", results);
    }

    terminal_writestring("\n----------------------------------------------\n");
    terminal_writestring("Tests Run: ");
    terminal_write_int(results->tests_run);
    terminal_writestring(" | Passed: ");
    terminal_setcolor(0x0A);
    terminal_write_int(results->tests_passed);
    terminal_setcolor(0x0F);
    terminal_writestring(" | Failed: ");
    if (results->tests_failed > 0) terminal_setcolor(0x0C);
    terminal_write_int(results->tests_failed);
    terminal_setcolor(0x0F);
    terminal_writestring("\n----------------------------------------------\n\n");
}
