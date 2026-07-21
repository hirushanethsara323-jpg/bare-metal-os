#ifndef HDA_H
#define HDA_H

#include <stdint.h>
#include <stdbool.h>

#define HDA_PCI_CLASS     0x04
#define HDA_PCI_SUBCLASS  0x03

#define HDA_REG_GCAP      0x00
#define HDA_REG_GCTL      0x08
#define HDA_REG_STATESTS  0x0E

/* Function Declarations */
bool hda_init(void);
uint32_t hda_get_capabilities(void);
bool hda_is_active(void);

#endif /* HDA_H */
