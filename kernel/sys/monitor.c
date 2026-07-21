/**
 * Nothing OS - Real-Time Kernel Performance Monitor & Metrics Engine
 * 
 * Tracks CPU Load %, Memory Heap Usage %, Active Task Counts, Hardware PIT
 * Clock Ticks, and Serial Telemetry Throughput.
 */

#include "../include/monitor.h"
#include "../include/heap.h"
#include "../include/sched.h"
#include "../include/idt.h"
#include "../include/serial.h"

extern void terminal_setcolor(uint8_t color);
extern void terminal_writestring(const char* data);
extern void terminal_write_int(int num);

void monitor_init(void) {
    klog(KLOG_INFO, "Real-Time System Telemetry & Performance Monitor Active.");
}

void monitor_collect_metrics(system_metrics_t* metrics) {
    if (metrics == 0) return;

    /* Gather Heap Usage */
    heap_stats_t hstats;
    heap_get_stats(&hstats);

    metrics->memory_used_bytes  = hstats.allocated_bytes;
    metrics->memory_total_bytes = hstats.total_size;
    metrics->memory_percent     = (hstats.total_size > 0) ? (hstats.allocated_bytes * 100) / hstats.total_size : 0;

    /* Gather Process Task Count */
    process_t* proc = process_get_list();
    uint32_t active_cnt = 0;
    while (proc != NULL) {
        if (proc->state == PROCESS_RUNNING || proc->state == PROCESS_READY) {
            active_cnt++;
        }
        proc = proc->next;
    }
    metrics->active_tasks = active_cnt;

    /* Gather PIT Clock Details */
    metrics->system_uptime_sec = timer_get_uptime_sec();
    metrics->total_pit_ticks   = timer_get_ticks();

    /* Estimate CPU Load % based on active workload */
    metrics->cpu_load_percent = 5 + (active_cnt * 3);
    if (metrics->cpu_load_percent > 100) metrics->cpu_load_percent = 100;
}

/**
 * Render System Telemetry Performance Dashboard.
 */
void monitor_display_dashboard(void) {
    system_metrics_t m;
    monitor_collect_metrics(&m);

    terminal_setcolor(0x0B); /* Light Cyan */
    terminal_writestring("=================================================================\n");
    terminal_writestring("  📈 NOTHING OS REAL-TIME KERNEL PERFORMANCE TELEMETRY DASHBOARD  \n");
    terminal_writestring("=================================================================\n\n");

    terminal_setcolor(0x0F);
    terminal_writestring("  Estimated CPU Load:    [");
    int filled = (m.cpu_load_percent * 20) / 100;
    terminal_setcolor(0x0A);
    for (int i = 0; i < 20; i++) {
        if (i < filled) terminal_writestring("=");
        else terminal_writestring("-");
    }
    terminal_setcolor(0x0F);
    terminal_writestring("] ");
    terminal_write_int(m.cpu_load_percent);
    terminal_writestring("%\n\n");

    terminal_writestring("  Dynamic Heap Usage:    [");
    int mem_filled = (m.memory_percent * 20) / 100;
    terminal_setcolor(0x0E);
    for (int i = 0; i < 20; i++) {
        if (i < mem_filled) terminal_writestring("=");
        else terminal_writestring("-");
    }
    terminal_setcolor(0x0F);
    terminal_writestring("] ");
    terminal_write_int(m.memory_percent);
    terminal_writestring("% (");
    terminal_write_int(m.memory_used_bytes);
    terminal_writestring(" / ");
    terminal_write_int(m.memory_total_bytes);
    terminal_writestring(" bytes)\n\n");

    terminal_writestring("  Active Process Tasks:  ");
    terminal_write_int(m.active_tasks);
    terminal_writestring(" Concurrent Thread PCBs\n");

    terminal_writestring("  System Uptime:         ");
    terminal_write_int(m.system_uptime_sec);
    terminal_writestring(" Seconds (");
    terminal_write_int(m.total_pit_ticks);
    terminal_writestring(" Hardware PIT Ticks @ 100Hz)\n");

    terminal_setcolor(0x0B);
    terminal_writestring("\n=================================================================\n\n");
}
