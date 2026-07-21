#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stdbool.h>

#define ELF_MAGIC 0x464C457F /* "\x7FELF" in Little-Endian */

/* ELF32 File Header Structure */
typedef struct {
    uint8_t  e_ident[16];   /* Magic identification & spec bytes */
    uint16_t e_type;        /* Executable file type */
    uint16_t e_machine;     /* Required architecture (0x03 = i386) */
    uint32_t e_version;     /* File version */
    uint32_t e_entry;       /* Program virtual entry point address */
    uint32_t e_phoff;       /* Program header table offset */
    uint32_t e_shoff;       /* Section header table offset */
    uint32_t e_flags;       /* Processor-specific flags */
    uint16_t e_ehsize;      /* ELF header size in bytes */
    uint16_t e_phentsize;   /* Program header table entry size */
    uint16_t e_phnum;       /* Number of program header entries */
    uint16_t e_shentsize;   /* Section header table entry size */
    uint16_t e_shnum;       /* Number of section header entries */
    uint16_t e_shstrndx;    /* Section header string table index */
} __attribute__((packed)) elf32_header_t;

/* Function Declarations */
bool elf_validate(const elf32_header_t* header);
void elf_inspect_header(const elf32_header_t* header);

#endif /* ELF_H */
