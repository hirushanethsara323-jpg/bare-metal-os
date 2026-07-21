/**
 * Nothing OS - PCI Bus Enumerator & Hardware Scanner Driver
 * 
 * Scans motherboard PCI buses 0..255, slots 0..31, and functions 0..7
 * via configuration ports 0xCF8 & 0xCFC to discover connected hardware adapters.
 */

#include "../include/pci.h"
#include "../include/io.h"
#include "../include/heap.h"
#include "../include/serial.h"

static pci_device_t detected_devices[32];
static uint32_t device_count = 0;

uint32_t pci_read_config_32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)((1 << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC));
    outb(PCI_CONFIG_ADDRESS, address & 0xFF);
    outb(PCI_CONFIG_ADDRESS + 1, (address >> 8) & 0xFF);
    outb(PCI_CONFIG_ADDRESS + 2, (address >> 16) & 0xFF);
    outb(PCI_CONFIG_ADDRESS + 3, (address >> 24) & 0xFF);

    uint32_t val;
    uint8_t b0 = inb(PCI_CONFIG_DATA);
    uint8_t b1 = inb(PCI_CONFIG_DATA + 1);
    uint8_t b2 = inb(PCI_CONFIG_DATA + 2);
    uint8_t b3 = inb(PCI_CONFIG_DATA + 3);
    val = (uint32_t)b0 | ((uint32_t)b1 << 8) | ((uint32_t)b2 << 16) | ((uint32_t)b3 << 24);

    return val;
}

uint16_t pci_read_config_16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t val = pci_read_config_32(bus, slot, func, offset);
    return (uint16_t)((val >> ((offset & 2) * 8)) & 0xFFFF);
}

void pci_scan_bus(void (*device_callback)(pci_device_t* dev)) {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint16_t vendor = pci_read_config_16((uint8_t)bus, slot, func, 0x00);
                if (vendor == 0xFFFF) continue; /* Device Not Present */

                uint16_t device = pci_read_config_16((uint8_t)bus, slot, func, 0x02);
                uint32_t class_reg = pci_read_config_32((uint8_t)bus, slot, func, 0x08);
                uint32_t bar0 = pci_read_config_32((uint8_t)bus, slot, func, 0x10);

                if (device_count < 32) {
                    pci_device_t* d = &detected_devices[device_count++];
                    d->bus = (uint8_t)bus;
                    d->slot = slot;
                    d->func = func;
                    d->vendor_id = vendor;
                    d->device_id = device;
                    d->class_code = (uint8_t)(class_reg >> 24);
                    d->subclass = (uint8_t)(class_reg >> 16);
                    d->bar0 = bar0;

                    if (device_callback != 0) {
                        device_callback(d);
                    }
                }
            }
        }
    }
}

pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id) {
    for (uint32_t i = 0; i < device_count; i++) {
        if (detected_devices[i].vendor_id == vendor_id && detected_devices[i].device_id == device_id) {
            return &detected_devices[i];
        }
    }
    return NULL;
}

void pci_init(void) {
    device_count = 0;
    pci_scan_bus(NULL);
    klog(KLOG_INFO, "PCI Hardware Bus Enumerator Scanned All Devices Successfully.");
}
