#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>

#define PAGE_PRESENT  0x1
#define PAGE_RW       0x2
#define PAGE_USER     0x4
#define PAGE_SIZE     4096

/* Page Table Entry (PTE) */
typedef uint32_t page_table_entry_t;

/* Page Directory Entry (PDE) */
typedef uint32_t page_directory_entry_t;

/* Page Table Structure (1024 Entries) */
typedef struct page_table {
    page_table_entry_t pages[1024];
} __attribute__((aligned(4096))) page_table_t;

/* Page Directory Structure (1024 Entries) */
typedef struct page_directory {
    page_directory_entry_t tables[1024];
} __attribute__((aligned(4096))) page_directory_t;

/* Function Declarations */
void paging_init(void);
void paging_map_page(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags);
void page_fault_handler(uint32_t error_code);
uint32_t paging_get_fault_address(void);

#endif /* PAGING_H */
