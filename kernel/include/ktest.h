#ifndef KTEST_H
#define KTEST_H

#include <stdint.h>
#include <stdbool.h>

/* Automated Kernel Test Suite Interface */
typedef struct {
    uint32_t tests_run;
    uint32_t tests_passed;
    uint32_t tests_failed;
} test_results_t;

void run_kernel_test_suite(test_results_t* results);

#endif /* KTEST_H */
