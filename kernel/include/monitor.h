#ifndef MONITOR_H
#define MONITOR_H

#include <stdint.h>
#include <stdbool.h>

/* System Performance Telemetry Metrics */
typedef struct {
    uint32_t cpu_load_percent;
    uint32_t memory_used_bytes;
    uint32_t memory_total_bytes;
    uint32_t memory_percent;
    uint32_t active_tasks;
    uint32_t system_uptime_sec;
    uint32_t total_pit_ticks;
} system_metrics_t;

/* Function Declarations */
void monitor_init(void);
void monitor_collect_metrics(system_metrics_t* metrics);
void monitor_display_dashboard(void);

#endif /* MONITOR_H */
