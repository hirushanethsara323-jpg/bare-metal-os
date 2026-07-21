#ifndef ISO9660_H
#define ISO9660_H

#include <stdint.h>
#include <stdbool.h>

#define ISO9660_MAGIC "CD001"

/* ISO-9660 Primary Volume Descriptor (PVD) Header Structure */
typedef struct {
    uint8_t type;               /* 0x01 = Primary Volume Descriptor */
    char    id[5];              /* Must be "CD001" */
    uint8_t version;            /* Must be 0x01 */
    uint8_t unused1;
    char    system_id[32];
    char    volume_id[32];
    uint8_t unused2[8];
    uint32_t volume_space_size_lsb;
    uint32_t volume_space_size_msb;
} __attribute__((packed)) iso9660_pvd_t;

/* Function Declarations */
bool iso9660_validate_pvd(const iso9660_pvd_t* pvd);
void iso9660_inspect_volume(const iso9660_pvd_t* pvd);

#endif /* ISO9660_H */
