#ifndef NVME_H
#define NVME_H

#include <stdint.h>
#include <stdbool.h>

#define NVME_PCI_CLASS    0x01
#define NVME_PCI_SUBCLASS 0x08

#define NVME_REG_CAP      0x00
#define NVME_REG_VS       0x08
#define NVME_REG_CC       0x14
#define NVME_REG_AQA      0x24

/* Function Declarations */
bool nvme_init(void);
uint64_t nvme_get_capabilities(void);
uint32_t nvme_get_version(void);

#endif /* NVME_H */
