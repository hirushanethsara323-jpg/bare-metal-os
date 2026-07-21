/**
 * Nothing OS - Dynamic Shared Memory (SHM) Inter-Thread Allocator
 * 
 * Manages shared physical memory buffers for concurrent process threads,
 * allowing multi-process read/write zero-copy data exchange across virtual spaces.
 */

#include "../include/shm.h"
#include "../include/heap.h"
#include "../include/serial.h"

static shm_segment_t* shm_head = NULL;

void shm_init(void) {
    shm_head = NULL;
    klog(KLOG_INFO, "Dynamic Shared Memory (SHM) Subsystem Initialized.");
}

void* shm_get(uint32_t key, size_t size) {
    if (key == 0 || size == 0) return NULL;

    /* Search for existing key segment */
    shm_segment_t* curr = shm_head;
    while (curr != NULL) {
        if (curr->key == key) {
            curr->attach_count++;
            return curr->mem_ptr;
        }
        curr = curr->next;
    }

    /* Allocate new shared memory segment */
    shm_segment_t* new_seg = (shm_segment_t*)kmalloc(sizeof(shm_segment_t));
    if (new_seg == NULL) return NULL;

    new_seg->mem_ptr = kmalloc(size);
    if (new_seg->mem_ptr == NULL) {
        kfree(new_seg);
        return NULL;
    }

    new_seg->key = key;
    new_seg->size = size;
    new_seg->attach_count = 1;
    new_seg->next = shm_head;
    shm_head = new_seg;

    klog(KLOG_INFO, "Created New Inter-Process Shared Memory Segment.");
    return new_seg->mem_ptr;
}

void* shm_at(uint32_t key) {
    shm_segment_t* curr = shm_head;
    while (curr != NULL) {
        if (curr->key == key) {
            return curr->mem_ptr;
        }
        curr = curr->next;
    }
    return NULL;
}

bool shm_dt(uint32_t key) {
    shm_segment_t* curr = shm_head;
    while (curr != NULL) {
        if (curr->key == key) {
            if (curr->attach_count > 0) {
                curr->attach_count--;
            }
            return true;
        }
        curr = curr->next;
    }
    return false;
}
