#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>
#include <stdbool.h>

/* Benchmark Report Results */
typedef struct {
    uint32_t memory_bandwidth_mbps;
    uint32_t cpu_mips_estimate;
    uint32_t disk_read_kbps;
} benchmark_results_t;

/* Function Declarations */
void run_system_benchmark(benchmark_results_t* results);
void benchmark_print_summary(void);

#endif /* BENCHMARK_H */
