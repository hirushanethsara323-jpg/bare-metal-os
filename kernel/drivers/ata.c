/**
 * Nothing OS - ATA IDE Hard Disk Sector Controller Driver
 * 
 * Provides 28-bit LBA sector reading and writing over Primary ATA Bus (0x1F0-0x1F7).
 */

#include "../include/ata.h"
#include "../include/io.h"
#include "../include/serial.h"

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static void ata_wait_busy(void) {
    while (inb(ATA_PRIMARY_COMM_STATUS) & 0x80);
}

static void ata_wait_drq(void) {
    while (!(inb(ATA_PRIMARY_COMM_STATUS) & 0x08));
}

void ata_init(void) {
    klog(KLOG_INFO, "Primary ATA IDE Hard Disk Controller Initialized.");
}

/**
 * Identify Primary Master ATA Drive.
 */
bool ata_identify(void) {
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
    outb(ATA_PRIMARY_SEC_COUNT, 0);
    outb(ATA_PRIMARY_LBA_LO, 0);
    outb(ATA_PRIMARY_LBA_MID, 0);
    outb(ATA_PRIMARY_LBA_HI, 0);
    outb(ATA_PRIMARY_COMM_STATUS, ATA_CMD_IDENTIFY);

    uint8_t status = inb(ATA_PRIMARY_COMM_STATUS);
    if (status == 0) return false; /* No drive */

    ata_wait_busy();

    if (inb(ATA_PRIMARY_LBA_MID) != 0 || inb(ATA_PRIMARY_LBA_HI) != 0) {
        return false; /* Non-ATA Drive */
    }

    ata_wait_drq();

    /* Read 256 words (512 bytes) drive specification packet */
    for (int i = 0; i < 256; i++) {
        inw(ATA_PRIMARY_DATA);
    }

    return true;
}

/**
 * Read 512-byte raw disk sector at given 28-bit LBA index.
 */
bool ata_read_sector(uint32_t lba, uint8_t* buffer) {
    if (buffer == 0) return false;

    ata_wait_busy();

    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SEC_COUNT, 1);
    outb(ATA_PRIMARY_LBA_LO, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HI, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_COMM_STATUS, ATA_CMD_READ_SECTORS);

    ata_wait_busy();
    ata_wait_drq();

    uint16_t* ptr = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        ptr[i] = inw(ATA_PRIMARY_DATA);
    }

    return true;
}

/**
 * Write 512-byte raw disk sector to given 28-bit LBA index.
 */
bool ata_write_sector(uint32_t lba, const uint8_t* buffer) {
    if (buffer == 0) return false;

    ata_wait_busy();

    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SEC_COUNT, 1);
    outb(ATA_PRIMARY_LBA_LO, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HI, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_COMM_STATUS, ATA_CMD_WRITE_SECTORS);

    ata_wait_busy();
    ata_wait_drq();

    const uint16_t* ptr = (const uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        outw(ATA_PRIMARY_DATA, ptr[i]);
    }

    /* Cache Flush Command */
    outb(ATA_PRIMARY_COMM_STATUS, 0xE7);
    ata_wait_busy();

    return true;
}
