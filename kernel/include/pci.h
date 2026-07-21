#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stdbool.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

/* PCI Device Structure */
typedef struct {
    uint8_t  bus;
    uint8_t  slot;
    uint8_t  func;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    uint8_t  header_type;
    uint32_t bar0;
} pci_device_t;

/* Function Declarations */
void pci_init(void);
uint32_t pci_read_config_32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_read_config_16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_scan_bus(void (*device_callback)(pci_device_t* dev));
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id);

#endif /* PCI_H */
