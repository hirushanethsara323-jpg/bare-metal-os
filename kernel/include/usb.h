#ifndef USB_H
#define USB_H

#include <stdint.h>
#include <stdbool.h>

#define USB_UHCI_CMD_RUN     0x0001
#define USB_UHCI_CMD_RESET   0x0002

/* Function Declarations */
bool usb_init(void);
uint16_t usb_read_port_status(uint8_t port_num);
bool usb_has_device_connected(void);

#endif /* USB_H */
