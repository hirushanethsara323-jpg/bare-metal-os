#ifndef RTL8139_H
#define RTL8139_H

#include <stdint.h>
#include <stdbool.h>

#define REALTEK_VENDOR_ID 0x10EC
#define RTL8139_DEV_ID    0x8139

/* Function Declarations */
bool rtl8139_init(void);
void rtl8139_get_mac(uint8_t mac[6]);
bool rtl8139_is_active(void);

#endif /* RTL8139_H */
