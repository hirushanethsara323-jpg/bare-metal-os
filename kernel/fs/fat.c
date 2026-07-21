/**
 * Nothing OS - FAT / MBR Boot Sector & BIOS Parameter Block (BPB) Parser
 * 
 * Inspects MBR partition signature (0xAA55), OEM formatting identifiers,
 * bytes per sector, cluster sizing, and FAT volume metadata.
 */

#include "../include/fat.h"
#include "../include/serial.h"

extern void terminal_setcolor(uint8_t color);
extern void terminal_writestring(const char* data);
extern void terminal_write_int(int num);
extern void terminal_putchar(char c);

bool fat_validate_bpb(const fat_bpb_t* bpb, uint16_t boot_sig) {
    if (bpb == 0) return false;

    if (boot_sig != FAT_BOOT_SIGNATURE) {
        klog(KLOG_ERROR, "FAT Validation Failed: Invalid 0xAA55 MBR Boot Signature.");
        return false;
    }

    if (bpb->bytes_per_sector != 512 && bpb->bytes_per_sector != 1024 && bpb->bytes_per_sector != 2048) {
        klog(KLOG_ERROR, "FAT Validation Failed: Unrecognized sector byte count.");
        return false;
    }

    klog(KLOG_INFO, "FAT Boot Sector & BPB Architecture Validated Successfully.");
    return true;
}

void fat_inspect_bpb(const fat_bpb_t* bpb, uint16_t boot_sig) {
    if (!fat_validate_bpb(bpb, boot_sig)) {
        terminal_setcolor(0x0C);
        terminal_writestring("  [FAIL] Boot sector missing valid 0xAA55 MBR signature!\n");
        return;
    }

    terminal_setcolor(0x0B);
    terminal_writestring("FAT Filesystem Boot Sector Metadata Specs:\n");
    terminal_setcolor(0x0F);
    terminal_writestring("  MBR Boot Signature: 0xAA55 (VALID)\n");
    terminal_writestring("  OEM Format Name:    ");
    for (int i = 0; i < 8; i++) terminal_putchar(bpb->oem_name[i]);
    terminal_writestring("\n  Bytes Per Sector:   ");
    terminal_write_int(bpb->bytes_per_sector);
    terminal_writestring("\n  Sectors Per Cluster:");
    terminal_write_int(bpb->sectors_per_cluster);
    terminal_writestring("\n  Reserved Sectors:   ");
    terminal_write_int(bpb->reserved_sectors);
    terminal_writestring("\n  FAT Allocation Tables: ");
    terminal_write_int(bpb->fat_count);
    terminal_writestring("\n  Root Directory Items:  ");
    terminal_write_int(bpb->root_dir_entries);
    terminal_writestring("\n");
}
