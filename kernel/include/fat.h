#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include <stdbool.h>

/* FAT12 / FAT16 / FAT32 BIOS Parameter Block (BPB) Structure */
typedef struct {
    uint8_t  boot_jmp[3];
    char     oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;
    uint16_t root_dir_entries;
    uint16_t total_sectors_short;
    uint8_t  media_descriptor;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t total_sectors_large;
} __attribute__((packed)) fat_bpb_t;

/* Boot Sector Signature Entry */
#define FAT_BOOT_SIGNATURE 0xAA55

/* Function Declarations */
bool fat_validate_bpb(const fat_bpb_t* bpb, uint16_t boot_sig);
void fat_inspect_bpb(const fat_bpb_t* bpb, uint16_t boot_sig);

#endif /* FAT_H */
