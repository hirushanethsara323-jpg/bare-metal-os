#ifndef SB16_H
#define SB16_H

#include <stdint.h>
#include <stdbool.h>

#define SB16_DEFAULT_BASE    0x220
#define SB16_DSP_RESET       0x226
#define SB16_DSP_READ        0x22A
#define SB16_DSP_WRITE       0x22C
#define SB16_DSP_READ_STATUS 0x22E

/* Function Declarations */
bool sb16_init(void);
bool sb16_reset_dsp(void);
void sb16_write_dsp(uint8_t value);
uint8_t sb16_read_dsp(void);

#endif /* SB16_H */
