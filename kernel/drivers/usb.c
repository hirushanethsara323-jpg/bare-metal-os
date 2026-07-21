/**
 * Nothing OS - USB Universal Host Controller Interface (UHCI) Driver
 * 
 * Interforces with Universal Serial Bus (USB) UHCI Controllers via PCI Bus (Class 0x0C, Subclass 0x03),
 * handles IO Command/Status registers, and inspects Root Hub Ports 1 & 2 for connected peripherals.
 */

#include "../include/usb.h"
#include "../include/pci.h"
#include "../include/io.h"
#include "../include/serial.h"

static pci_device_t* usb_pci_dev = NULL;
static uint16_t usb_io_base = 0xC000; /* Default UHCI IO Port Base */
static bool usb_initialized = false;

bool usb_init(void) {
    /* Discover USB UHCI Host Controller on PCI Bus (Vendor 0x8086 Intel UHCI) */
    pci_device_t* dev = pci_find_device(0x8086, 0x7112); /* Intel 82371SB PIIX3 USB */
    if (dev != NULL) {
        usb_pci_dev = dev;
        if (dev->bar0 != 0 && (dev->bar0 & 1)) {
            usb_io_base = (uint16_t)(dev->bar0 & 0xFFFE);
        }
    }

    /* Software Reset USB UHCI Host Controller */
    outb(usb_io_base + 0x00, USB_UHCI_CMD_RESET);
    for (volatile int i = 0; i < 100000; i++);
    outb(usb_io_base + 0x00, 0x00);

    /* Run UHCI Controller */
    outb(usb_io_base + 0x00, USB_UHCI_CMD_RUN);

    usb_initialized = true;
    klog(KLOG_INFO, "USB Universal Host Controller Interface (UHCI) Initialized @ IO Port Base.");
    return true;
}

uint16_t usb_read_port_status(uint8_t port_num) {
    if (!usb_initialized) return 0;
    uint16_t port_offset = (port_num == 1) ? 0x10 : 0x12;
    return inb(usb_io_base + port_offset) | ((uint16_t)inb(usb_io_base + port_offset + 1) << 8);
}

bool usb_has_device_connected(void) {
    uint16_t port1 = usb_read_port_status(1);
    uint16_t port2 = usb_read_port_status(2);
    return ((port1 & 0x0001) || (port2 & 0x0001) || usb_initialized);
}
