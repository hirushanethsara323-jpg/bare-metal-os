#ifndef AHCI_H
#define AHCI_H

#include <stdint.h>
#include <stdbool.h>

/* AHCI MMIO Register Offsets */
#define AHCI_REG_CAP   0x00
#define AHCI_REG_GHC   0x04
#define AHCI_REG_IS    0x08
#define AHCI_REG_PI    0x0C
#define AHCI_REG_VS    0x10

/* Function Declarations */
bool ahci_init(void);
uint32_t ahci_get_ports_implemented(void);
uint32_t ahci_get_capabilities(void);

#endif /* AHCI_H */
