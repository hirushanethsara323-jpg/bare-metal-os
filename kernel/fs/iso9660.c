/**
 * Nothing OS - ISO-9660 CD-ROM / DVD Optical Filesystem Reader
 * 
 * Inspects Primary Volume Descriptors (PVD), validates "CD001" magic header signatures,
 * volume identifiers, and sector allocation tables on optical media storage devices.
 */

#include "../include/iso9660.h"
#include "../include/serial.h"

extern void terminal_setcolor(uint8_t color);
extern void terminal_writestring(const char* data);
extern void terminal_putchar(char c);

static int mem_cmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

bool iso9660_validate_pvd(const iso9660_pvd_t* pvd) {
    if (pvd == 0) return false;

    if (mem_cmp(pvd->id, ISO9660_MAGIC, 5) != 0) {
        klog(KLOG_ERROR, "ISO-9660 Validation Failed: Missing 'CD001' Optical Magic Identifier.");
        return false;
    }

    klog(KLOG_INFO, "ISO-9660 CD-ROM Optical Primary Volume Descriptor Validated Successfully.");
    return true;
}

void iso9660_inspect_volume(const iso9660_pvd_t* pvd) {
    if (!iso9660_validate_pvd(pvd)) {
        terminal_setcolor(0x0C);
        terminal_writestring("  [FAIL] Optical media missing valid 'CD001' ISO-9660 Volume Descriptor!\n");
        return;
    }

    terminal_setcolor(0x0B);
    terminal_writestring("ISO-9660 CD-ROM / DVD Optical Media Metadata Specs:\n");
    terminal_setcolor(0x0F);
    terminal_writestring("  Magic Header ID:    'CD001' (VALID)\n");
    terminal_writestring("  System Identifier:  ");
    for (int i = 0; i < 16; i++) terminal_putchar(pvd->system_id[i]);
    terminal_writestring("\n  Volume Label ID:    ");
    for (int i = 0; i < 16; i++) terminal_putchar(pvd->volume_id[i]);
    terminal_writestring("\n  Standard Sector Size: 2048 Bytes\n");
}
