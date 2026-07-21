#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define HEAP_MAGIC         0xDEADBEEF
#define HEAP_INITIAL_SIZE  (1 * 1024 * 1024) /* 1 MB Initial Heap */

/* Header for each block in heap */
typedef struct heap_block_header {
    uint32_t magic;                     /* Safety check magic number */
    bool is_free;                       /* Free or allocated flag */
    size_t size;                        /* Payload size in bytes */
    struct heap_block_header* next;     /* Pointer to next block in heap */
} heap_block_header_t;

/* Heap Statistics Structure */
typedef struct {
    size_t total_size;
    size_t allocated_bytes;
    size_t free_bytes;
    size_t total_blocks;
    size_t free_blocks;
} heap_stats_t;

/* Function Prototypes */
void heap_init(uintptr_t base_addr, size_t initial_size);
void* kmalloc(size_t size);
void* kzalloc(size_t size);
void* krealloc(void* ptr, size_t new_size);
void kfree(void* ptr);
void heap_get_stats(heap_stats_t* stats);

#endif /* HEAP_H */
