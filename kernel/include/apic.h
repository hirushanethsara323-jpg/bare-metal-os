#ifndef APIC_H
#define APIC_H

#include <stdint.h>
#include <stdbool.h>

#define APIC_BASE_MMIO           0xFEE00000
#define APIC_REG_ID              0x0020
#define APIC_REG_VERSION         0x0030
#define APIC_REG_TPR             0x0080
#define APIC_REG_EOI             0x00B0
#define APIC_REG_SVR             0x00F0
#define APIC_REG_ICR_LOW         0x0300
#define APIC_REG_ICR_HIGH        0x0310

/* Function Declarations */
bool apic_init(void);
uint32_t apic_read_register(uint32_t reg);
void apic_write_register(uint32_t reg, uint32_t value);
uint8_t apic_get_core_id(void);
void apic_eoi(void);

#endif /* APIC_H */
