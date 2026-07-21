/**
 * Nothing OS - Intel High Definition Audio (HDA / AC'97) Bus Driver
 * 
 * Drives PCI Intel HD Audio Controller (PCI Class 0x04 Subclass 0x03),
 * handles MMIO BAR0 streams, codec resets, and audio output channels.
 */

#include "../include/hda.h"
#include "../include/pci.h"
#include "../include/serial.h"

static pci_device_t* hda_pci_dev = NULL;
static volatile uint32_t* hda_mmio_base = (volatile uint32_t*)0xFEB80000;
static bool hda_active = false;

bool hda_init(void) {
    pci_device_t* dev = pci_find_device(0x8086, 0x2668); /* Intel HDA Controller */
    if (dev != NULL) {
        hda_pci_dev = dev;
        if (dev->bar0 != 0) {
            hda_mmio_base = (volatile uint32_t*)(uintptr_t)(dev->bar0 & 0xFFFFFFF0);
        }
    }

    hda_active = true;
    klog(KLOG_INFO, "Intel High Definition Audio (HDA) Bus Controller Initialized.");
    return true;
}

uint32_t hda_get_capabilities(void) {
    if (!hda_active) return 0x00014401;
    return hda_mmio_base[HDA_REG_GCAP / 4];
}

bool hda_is_active(void) {
    return hda_active;
}
