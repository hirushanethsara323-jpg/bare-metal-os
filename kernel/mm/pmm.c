/**
 * BareMetal OS - Physical Memory Manager (PMM)
 * 
 * Manages physical memory allocation.
 * Uses a bitmap-based allocation scheme.
 */

#include <stdint.h>

/* Memory layout */
#define KERNEL_START  0x100000
#define KERNEL_END    0x200000
#define BITMAP_START  0x200000
#define MAX_MEMORY    0x40000000  /* 1GB maximum for simplicity */

/* Bitmap configuration */
#define BITS_PER_BYTE 8
#define BITS_PER_PAGE (BITS_PER_BYTE * sizeof(uint32_t))
#define PAGE_SIZE     4096

/* Memory statistics */
static uint32_t total_memory = 0;
static uint32_t used_memory = 0;
static uint32_t free_memory = 0;

/* Memory bitmap */
static uint32_t* memory_bitmap = (uint32_t*)BITMAP_START;
static uint32_t total_frames = 0;
static uint32_t used_frames = 0;

/* Initialize PMM */
void pmm_init(uint32_t mem_size) {
    total_memory = mem_size;
    total_frames = mem_size / PAGE_SIZE;
    used_frames = KERNEL_END / PAGE_SIZE;  /* Kernel uses first part */
    
    /* Mark kernel pages as used */
    for (uint32_t i = 0; i < used_frames; i++) {
        memory_bitmap[i / 32] |= (1 << (i % 32));
    }
    
    /* Mark rest as free */
    for (uint32_t i = used_frames / 32 + 1; i < (total_frames / 32 + 1); i++) {
        memory_bitmap[i] = 0;
    }
    
    free_memory = total_memory - (used_frames * PAGE_SIZE);
    
    /* Debug output would go here */
}

/* Allocate a physical frame */
uint32_t pmm_alloc_frame(void) {
    /* Find first free frame */
    for (uint32_t i = used_frames / 32 + 1; i < (total_frames / 32 + 1); i++) {
        if (memory_bitmap[i] != 0xFFFFFFFF) {
            /* Found a block with free frames */
            for (int bit = 0; bit < 32; bit++) {
                if (!(memory_bitmap[i] & (1 << bit))) {
                    uint32_t frame = (i * 32) + bit;
                    
                    /* Check if frame is within bounds */
                    if (frame * PAGE_SIZE < total_memory) {
                        memory_bitmap[i] |= (1 << bit);
                        used_frames++;
                        return frame * PAGE_SIZE;
                    }
                    return 0;  /* Out of memory */
                }
            }
        }
    }
    return 0;  /* No free frames */
}

/* Free a physical frame */
void pmm_free_frame(uint32_t addr) {
    uint32_t frame = addr / PAGE_SIZE;
    uint32_t index = frame / 32;
    uint32_t bit = frame % 32;
    
    if (index < (total_frames / 32 + 1)) {
        memory_bitmap[index] &= ~(1 << bit);
        used_frames--;
    }
}

/* Check if frame is allocated */
int pmm_is_allocated(uint32_t addr) {
    uint32_t frame = addr / PAGE_SIZE;
    uint32_t index = frame / 32;
    uint32_t bit = frame % 32;
    
    return (memory_bitmap[index] & (1 << bit)) != 0;
}

/* Get memory statistics */
void pmm_get_stats(uint32_t* total, uint32_t* used, uint32_t* free) {
    if (total) *total = total_memory;
    if (used) *used = used_frames * PAGE_SIZE;
    if (free) *free = (total_frames - used_frames) * PAGE_SIZE;
}

/* Set memory region as used */
void pmm_reserve(uint32_t start, uint32_t end) {
    uint32_t start_frame = start / PAGE_SIZE;
    uint32_t end_frame = end / PAGE_SIZE;
    
    for (uint32_t i = start_frame; i <= end_frame && i < total_frames; i++) {
        memory_bitmap[i / 32] |= (1 << (i % 32));
        used_frames++;
    }
}

/* Debug: dump memory bitmap */
void pmm_dump(void) {
    volatile uint32_t* vga = (uint32_t*)0xB8000;
    volatile uint32_t* ptr = vga;
    
    /* Simple memory info display */
    (void)ptr;  /* Avoid unused warning */
    
    /* In real implementation, this would print to terminal */
}
