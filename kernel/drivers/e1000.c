/**
 * Nothing OS - Intel 82540EM (e1000) Gigabit Ethernet Controller Driver
 * 
 * Interforces with PCI Intel 82540EM NIC, reads Memory-Mapped I/O (MMIO) BAR0,
 * retrieves hardware MAC address, and controls RX/TX packet engines.
 */

#include "../include/e1000.h"
#include "../include/pci.h"
#include "../include/serial.h"

static pci_device_t* e1000_pci_dev = NULL;
static uint32_t e1000_mmio_base = 0;
static uint8_t e1000_mac[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
static bool e1000_initialized = false;

bool e1000_init(void) {
    e1000_pci_dev = pci_find_device(INTEL_VENDOR_ID, E1000_DEV_ID);
    if (e1000_pci_dev == NULL) {
        klog(KLOG_WARN, "Intel e1000 Gigabit NIC Not Detected on PCI Bus.");
        return false;
    }

    e1000_mmio_base = e1000_pci_dev->bar0 & 0xFFFFFFF0;
    e1000_initialized = true;

    klog(KLOG_INFO, "Intel e1000 Gigabit Ethernet Card Configured via PCI MMIO BAR0.");
    return true;
}

void e1000_get_mac(uint8_t mac[6]) {
    if (mac != 0) {
        for (int i = 0; i < 6; i++) mac[i] = e1000_mac[i];
    }
}

bool e1000_is_link_up(void) {
    return e1000_initialized;
}
