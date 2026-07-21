/**
 * Nothing OS - ELF32 Binary Executable Format Engine & Program Loader Validation
 * 
 * Verifies 32-bit x86 ELF binary headers, magic identification, i386 machine type,
 * program header table offsets, and virtual execution entry point addresses.
 */

#include "../include/elf.h"
#include "../include/serial.h"

extern void terminal_setcolor(uint8_t color);
extern void terminal_writestring(const char* data);
extern void terminal_write_hex(uint32_t num);
extern void terminal_write_int(int num);

/**
 * Validate ELF32 header magic bytes and x86 target architecture.
 */
bool elf_validate(const elf32_header_t* header) {
    if (header == 0) return false;

    /* Check Magic Header Signature 0x7F 'E' 'L' 'F' */
    uint32_t magic = *(const uint32_t*)header->e_ident;
    if (magic != ELF_MAGIC) {
        klog(KLOG_ERROR, "ELF Validation Failed: Invalid Magic Header Signature.");
        return false;
    }

    /* Check 32-bit Class (index 4 == 1) */
    if (header->e_ident[4] != 1) {
        klog(KLOG_ERROR, "ELF Validation Failed: Not a 32-bit ELF binary.");
        return false;
    }

    /* Check Target Machine (0x03 == Intel i386) */
    if (header->e_machine != 3) {
        klog(KLOG_ERROR, "ELF Validation Failed: Target machine is not Intel i386.");
        return false;
    }

    klog(KLOG_INFO, "ELF32 Executable Binary Header Validated Successfully.");
    return true;
}

/**
 * Display ELF32 Binary Inspection Telemetry.
 */
void elf_inspect_header(const elf32_header_t* header) {
    if (!elf_validate(header)) {
        terminal_setcolor(0x0C);
        terminal_writestring("  [FAIL] Binary is not a valid 32-bit x86 ELF Executable!\n");
        return;
    }

    terminal_setcolor(0x0B);
    terminal_writestring("ELF32 Executable Binary Header Specs:\n");
    terminal_setcolor(0x0F);
    terminal_writestring("  Magic Signature:    0x7F 'E' 'L' 'F' (VALID)\n");
    terminal_writestring("  Class:              32-bit ELF Executable\n");
    terminal_writestring("  Machine Arch:       Intel i386 (0x0003)\n");
    terminal_writestring("  Entry Address:      0x");
    terminal_write_hex(header->e_entry);
    terminal_writestring("\n  Program Headers:    ");
    terminal_write_int(header->e_phnum);
    terminal_writestring(" Entries @ Offset 0x");
    terminal_write_hex(header->e_phoff);
    terminal_writestring("\n  Section Headers:    ");
    terminal_write_int(header->e_shnum);
    terminal_writestring(" Entries @ Offset 0x");
    terminal_write_hex(header->e_shoff);
    terminal_writestring("\n");
}
