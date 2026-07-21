#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>
#include <stdbool.h>

#define ACPI_RSDP_SIGNATURE "RSD PTR "

/* Root System Description Pointer Structure */
typedef struct {
    char     signature[8];
    uint8_t  checksum;
    char     oem_id[6];
    uint8_t  revision;
    uint32_t rsdt_address;
} __attribute__((packed)) acpi_rsdp_t;

/* Function Declarations */
bool acpi_init(void);
acpi_rsdp_t* acpi_find_rsdp(void);
void acpi_poweroff(void);

#endif /* ACPI_H */
