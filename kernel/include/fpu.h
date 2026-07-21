#ifndef FPU_H
#define FPU_H

#include <stdint.h>
#include <stdbool.h>

/* Function Declarations */
bool fpu_init(void);
bool sse_init(void);
bool fpu_is_enabled(void);

#endif /* FPU_H */
