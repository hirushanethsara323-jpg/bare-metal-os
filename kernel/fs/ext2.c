/**
 * Nothing OS - Linux Ext2 Filesystem Driver & Superblock Parser
 * 
 * Inspects Linux Ext2 filesystem structures, block group allocation tables,
 * inodes, directory records, and validates 0xEF53 filesystem magic signatures.
 */

#include "../include/ext2.h"
#include "../include/serial.h"

extern void terminal_setcolor(uint8_t color);
extern void terminal_writestring(const char* data);
extern void terminal_write_int(int num);

bool ext2_validate_superblock(const ext2_superblock_t* sb) {
    if (sb == 0) return false;

    if (sb->s_magic != EXT2_SUPER_MAGIC) {
        klog(KLOG_ERROR, "Ext2 Validation Failed: Invalid 0xEF53 Superblock Magic Signature.");
        return false;
    }

    klog(KLOG_INFO, "Linux Ext2 Filesystem Superblock Validated Successfully.");
    return true;
}

void ext2_inspect_superblock(const ext2_superblock_t* sb) {
    if (!ext2_validate_superblock(sb)) {
        terminal_setcolor(0x0C);
        terminal_writestring("  [FAIL] Storage volume missing valid 0xEF53 Ext2 Superblock signature!\n");
        return;
    }

    terminal_setcolor(0x0B);
    terminal_writestring("Linux Ext2 Filesystem Volume Superblock Specs:\n");
    terminal_setcolor(0x0F);
    terminal_writestring("  Superblock Magic:   0xEF53 (VALID)\n");
    terminal_writestring("  Total Inodes:       ");
    terminal_write_int(sb->s_inodes_count);
    terminal_writestring("\n  Total Blocks:       ");
    terminal_write_int(sb->s_blocks_count);
    terminal_writestring("\n  Free Inodes:        ");
    terminal_write_int(sb->s_free_inodes_count);
    terminal_writestring("\n  Free Blocks:        ");
    terminal_write_int(sb->s_free_blocks_count);
    terminal_writestring("\n  Block Size:         ");
    terminal_write_int(1024 << sb->s_log_block_size);
    terminal_writestring(" Bytes\n  Mount Count:        ");
    terminal_write_int(sb->s_mnt_count);
    terminal_writestring("\n");
}
