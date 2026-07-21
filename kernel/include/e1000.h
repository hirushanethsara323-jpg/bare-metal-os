#ifndef E1000_H
#define E1000_H

#include <stdint.h>
#include <stdbool.h>

#define INTEL_VENDOR_ID 0x8086
#define E1000_DEV_ID    0x100E

/* e1000 MMIO Register Offsets */
#define E1000_REG_CTRL   0x0000
#define E1000_REG_STATUS 0x0008
#define E1000_REG_EERD   0x0018
#define E1000_REG_RAL0   0x5400
#define E1000_REG_RAH0   0x5404

/* Function Declarations */
bool e1000_init(void);
void e1000_get_mac(uint8_t mac[6]);
bool e1000_is_link_up(void);

#endif /* E1000_H */
