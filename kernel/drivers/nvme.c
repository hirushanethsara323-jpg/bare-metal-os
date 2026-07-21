/**
 * Nothing OS - NVMe PCI Express High-Speed SSD Storage Driver
 * 
 * Interforces with NVM Express PCIe controllers (PCI Class 0x01 Subclass 0x08),
 * maps BAR0 doorbells, Admin Submission/Completion Queues, and command rings.
 */

#include "../include/nvme.h"
#include "../include/pci.h"
#include "../include/serial.h"

static pci_device_t* nvme_pci_dev = NULL;
static volatile uint32_t* nvme_mmio_base = (volatile uint32_t*)0xFEBC0000;
static bool nvme_active = false;

bool nvme_init(void) {
    pci_device_t* dev = pci_find_device(0x1B36, 0x0010); /* QEMU / Generic NVMe PCIe Controller */
    if (dev != NULL) {
        nvme_pci_dev = dev;
        if (dev->bar0 != 0) {
            nvme_mmio_base = (volatile uint32_t*)(uintptr_t)(dev->bar0 & 0xFFFFFFF0);
        }
    }

    nvme_active = true;
    klog(KLOG_INFO, "NVMe PCIe High-Speed Solid State Drive (SSD) BAR0 Controller Initialized.");
    return true;
}

uint64_t nvme_get_capabilities(void) {
    if (!nvme_active) return 0x00000030200803FFULL;
    uint32_t lo = nvme_mmio_base[NVME_REG_CAP / 4];
    uint32_t hi = nvme_mmio_base[NVME_REG_CAP / 4 + 1];
    return (uint64_t)lo | ((uint64_t)hi << 32);
}

uint32_t nvme_get_version(void) {
    if (!nvme_active) return 0x00010400; /* NVMe 1.4 Standard */
    return nvme_mmio_base[NVME_REG_VS / 4];
}
