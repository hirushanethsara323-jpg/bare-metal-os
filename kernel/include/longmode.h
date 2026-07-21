#ifndef LONGMODE_H
#define LONGMODE_H

#include <stdint.h>
#include <stdbool.h>

#define IA32_EFER_MSR 0xC0000080
#define EFER_LME      (1 << 8)

/* PML4 Page Table Alignment (4KB) */
typedef struct {
    uint64_t entries[512];
} __attribute__((aligned(4096))) pml4_table_t;

/* Function Declarations */
bool longmode_check_support(void);
void longmode_init_pml4(void);
void longmode_inspect_state(void);

#endif /* LONGMODE_H */
