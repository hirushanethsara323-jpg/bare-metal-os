#ifndef EXT4_H
#define EXT4_H

#include <stdint.h>
#include <stdbool.h>

#define EXT4_FEATURE_INCOMPAT_EXTENTS 0x0040
#define EXT4_FEATURE_INCOMPAT_64BIT   0x0080

/* Ext4 Extent Header Structure */
typedef struct {
    uint16_t eh_magic;      /* Magic 0xF30A */
    uint16_t eh_entries;    /* Number of valid entries */
    uint16_t eh_max;        /* Capacity of entries */
    uint16_t eh_depth;      /* Tree depth (0 = leaf) */
    uint32_t eh_generation;
} __attribute__((packed)) ext4_extent_header_t;

/* Function Declarations */
bool ext4_init(void);
bool ext4_validate_extent_header(const ext4_extent_header_t* header);
void ext4_inspect_journal(void);

#endif /* EXT4_H */
