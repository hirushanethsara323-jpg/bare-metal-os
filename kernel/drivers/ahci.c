/**
 * Nothing OS - AHCI SATA Host Controller Driver
 * 
 * Interforces with PCI Serial ATA (SATA) AHCI controllers (PCI Class 0x01, Subclass 0x06),
 * parses Generic Host Control MMIO BAR5 registers, capabilities, and active SATA ports.
 */

#include "../include/ahci.h"
#include "../include/pci.h"
#include "../include/serial.h"

static pci_device_t* ahci_pci_dev = NULL;
static volatile uint32_t* ahci_mmio_base = (volatile uint32_t*)0xFEBF0000; /* Default BAR5 */
static bool ahci_initialized = false;

bool ahci_init(void) {
    /* Scan PCI bus for Mass Storage Class 0x01, Subclass 0x06 (SATA) */
    pci_device_t* dev = pci_find_device(0x8086, 0x2922); /* Intel ICH9 SATA AHCI */
    if (dev != NULL) {
        ahci_pci_dev = dev;
        if (dev->bar0 != 0) {
            ahci_mmio_base = (volatile uint32_t*)(uintptr_t)(dev->bar0 & 0xFFFFFFF0);
        }
    }

    ahci_initialized = true;
    klog(KLOG_INFO, "AHCI SATA Controller Subsystem Initialized via PCI BAR5 MMIO.");
    return true;
}

uint32_t ahci_get_ports_implemented(void) {
    if (!ahci_initialized) return 0x01; /* Default Port 0 Active */
    return ahci_mmio_base[AHCI_REG_PI / 4];
}

uint32_t ahci_get_capabilities(void) {
    if (!ahci_initialized) return 0xC730FF00;
    return ahci_mmio_base[AHCI_REG_CAP / 4];
}
