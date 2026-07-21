#ifndef SHM_H
#define SHM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Shared Memory Segment Node */
typedef struct shm_segment {
    uint32_t key;
    size_t size;
    void* mem_ptr;
    uint32_t attach_count;
    struct shm_segment* next;
} shm_segment_t;

/* Function Declarations */
void shm_init(void);
void* shm_get(uint32_t key, size_t size);
void* shm_at(uint32_t key);
bool shm_dt(uint32_t key);

#endif /* SHM_H */
