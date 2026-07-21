/**
 * Nothing OS - Dynamic Kernel Heap Allocator
 * 
 * Implements a boundary-tag first-fit memory allocator with automatic block
 * splitting, free-list coalescing, kmalloc, kzalloc, krealloc, and kfree.
 */

#include "../include/heap.h"

static heap_block_header_t* heap_head = NULL;
static size_t heap_total_size = 0;

/**
 * Initialize Kernel Heap starting at base_addr with given initial_size.
 */
void heap_init(uintptr_t base_addr, size_t initial_size) {
    /* Align base address to 8 bytes */
    if (base_addr % 8 != 0) {
        base_addr += 8 - (base_addr % 8);
    }
    
    heap_head = (heap_block_header_t*)base_addr;
    heap_head->magic = HEAP_MAGIC;
    heap_head->is_free = true;
    heap_head->size = initial_size - sizeof(heap_block_header_t);
    heap_head->next = NULL;
    
    heap_total_size = initial_size;
}

/**
 * Coalesce adjacent free heap blocks to prevent memory fragmentation.
 */
static void heap_coalesce(void) {
    heap_block_header_t* curr = heap_head;
    
    while (curr != NULL && curr->next != NULL) {
        if (curr->is_free && curr->next->is_free) {
            /* Merge curr with curr->next */
            curr->size += sizeof(heap_block_header_t) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

/**
 * Allocate dynamic memory block of requested size in bytes.
 */
void* kmalloc(size_t size) {
    if (size == 0 || heap_head == NULL) {
        return NULL;
    }
    
    /* 8-byte alignment for hardware performance */
    if (size % 8 != 0) {
        size += 8 - (size % 8);
    }
    
    heap_block_header_t* curr = heap_head;
    
    while (curr != NULL) {
        /* Verify block integrity */
        if (curr->magic != HEAP_MAGIC) {
            /* Heap corruption detected! */
            return NULL;
        }
        
        if (curr->is_free && curr->size >= size) {
            /* Check if we can split this block */
            if (curr->size >= size + sizeof(heap_block_header_t) + 16) {
                uintptr_t split_addr = (uintptr_t)curr + sizeof(heap_block_header_t) + size;
                heap_block_header_t* split_block = (heap_block_header_t*)split_addr;
                
                split_block->magic = HEAP_MAGIC;
                split_block->is_free = true;
                split_block->size = curr->size - size - sizeof(heap_block_header_t);
                split_block->next = curr->next;
                
                curr->size = size;
                curr->next = split_block;
            }
            
            curr->is_free = false;
            return (void*)((uintptr_t)curr + sizeof(heap_block_header_t));
        }
        
        curr = curr->next;
    }
    
    /* Out of heap memory */
    return NULL;
}

/**
 * Allocate memory and zero initialize all bytes.
 */
void* kzalloc(size_t size) {
    void* ptr = kmalloc(size);
    if (ptr != NULL) {
        uint8_t* byte_ptr = (uint8_t*)ptr;
        for (size_t i = 0; i < size; i++) {
            byte_ptr[i] = 0;
        }
    }
    return ptr;
}

/**
 * Reallocate memory block to a new size.
 */
void* krealloc(void* ptr, size_t new_size) {
    if (ptr == NULL) {
        return kmalloc(new_size);
    }
    
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    heap_block_header_t* block = (heap_block_header_t*)((uintptr_t)ptr - sizeof(heap_block_header_t));
    if (block->magic != HEAP_MAGIC) {
        return NULL;
    }
    
    if (block->size >= new_size) {
        return ptr;
    }
    
    void* new_ptr = kmalloc(new_size);
    if (new_ptr != NULL) {
        uint8_t* src = (uint8_t*)ptr;
        uint8_t* dst = (uint8_t*)new_ptr;
        for (size_t i = 0; i < block->size; i++) {
            dst[i] = src[i];
        }
        kfree(ptr);
    }
    
    return new_ptr;
}

/**
 * Free previously allocated heap memory block.
 */
void kfree(void* ptr) {
    if (ptr == NULL || heap_head == NULL) {
        return;
    }
    
    heap_block_header_t* block = (heap_block_header_t*)((uintptr_t)ptr - sizeof(heap_block_header_t));
    
    /* Check magic number to protect against invalid free calls */
    if (block->magic != HEAP_MAGIC) {
        return;
    }
    
    block->is_free = true;
    
    /* Merge free contiguous memory blocks */
    heap_coalesce();
}

/**
 * Get current Kernel Heap statistics.
 */
void heap_get_stats(heap_stats_t* stats) {
    if (stats == NULL) return;
    
    stats->total_size = heap_total_size;
    stats->allocated_bytes = 0;
    stats->free_bytes = 0;
    stats->total_blocks = 0;
    stats->free_blocks = 0;
    
    heap_block_header_t* curr = heap_head;
    while (curr != NULL) {
        stats->total_blocks++;
        if (curr->is_free) {
            stats->free_blocks++;
            stats->free_bytes += curr->size;
        } else {
            stats->allocated_bytes += curr->size;
        }
        curr = curr->next;
    }
}
