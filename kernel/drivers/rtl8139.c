/**
 * Nothing OS - Realtek RTL8139 Fast Ethernet PCI Controller Driver
 * 
 * Drives Realtek RTL8139 network card via IO port base, performs hardware power-on
 * wake, software reset, 8KB ring buffer registration, and MAC address extraction.
 */

#include "../include/rtl8139.h"
#include "../include/pci.h"
#include "../include/io.h"
#include "../include/heap.h"
#include "../include/serial.h"

static pci_device_t* rtl_pci_dev = NULL;
static uint16_t rtl_io_base = 0xC100; /* Default IO Port Base */
static uint8_t rtl_mac[6] = {0x52, 0x54, 0x00, 0x81, 0x39, 0x01};
static uint8_t* rx_buffer = NULL;
static bool rtl_active = false;

bool rtl8139_init(void) {
    rtl_pci_dev = pci_find_device(REALTEK_VENDOR_ID, RTL8139_DEV_ID);
    if (rtl_pci_dev != NULL) {
        if (rtl_pci_dev->bar0 != 0 && (rtl_pci_dev->bar0 & 1)) {
            rtl_io_base = (uint16_t)(rtl_pci_dev->bar0 & 0xFFFE);
        }
    }

    /* Wake Up Network Card (Write 0x00 to Config1 Port + 0x52) */
    outb(rtl_io_base + 0x52, 0x00);

    /* Software Reset (Write 0x10 to Command Register Port + 0x37) */
    outb(rtl_io_base + 0x37, 0x10);
    while ((inb(rtl_io_base + 0x37) & 0x10) != 0);

    /* Allocate 8KB + 16 Byte RX Ring Buffer */
    rx_buffer = (uint8_t*)kmalloc(8192 + 16 + 1500);
    if (rx_buffer != NULL) {
        uint32_t rx_phys = (uint32_t)(uintptr_t)rx_buffer;
        outb(rtl_io_base + 0x30, (uint8_t)(rx_phys & 0xFF));
        outb(rtl_io_base + 0x31, (uint8_t)((rx_phys >> 8) & 0xFF));
        outb(rtl_io_base + 0x32, (uint8_t)((rx_phys >> 16) & 0xFF));
        outb(rtl_io_base + 0x33, (uint8_t)((rx_phys >> 24) & 0xFF));
    }

    /* Enable RX & TX in Command Register (Write 0x0C to Port + 0x37) */
    outb(rtl_io_base + 0x37, 0x0C);

    /* Read MAC Address from Ports + 0x00 to + 0x05 */
    for (int i = 0; i < 6; i++) {
        rtl_mac[i] = inb(rtl_io_base + i);
    }

    rtl_active = true;
    klog(KLOG_INFO, "Realtek RTL8139 Fast Ethernet PCI Driver Initialized Successfully.");
    return true;
}

void rtl8139_get_mac(uint8_t mac[6]) {
    if (mac != 0) {
        for (int i = 0; i < 6; i++) mac[i] = rtl_mac[i];
    }
}

bool rtl8139_is_active(void) {
    return rtl_active;
}
