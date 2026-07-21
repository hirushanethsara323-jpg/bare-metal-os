/**
 * Nothing OS - Inter-Process Communication (IPC) Pipes & Semaphore Synchronization
 * 
 * Provides ring-buffer pipe message passing between process threads and
 * counting semaphore locks for mutual exclusion and concurrency control.
 */

#include "../include/ipc.h"
#include "../include/serial.h"

void ipc_init(void) {
    klog(KLOG_INFO, "IPC Pipe Subsystem & Counting Semaphore Engine Initialized.");
}

void pipe_init(pipe_t* p) {
    if (p == 0) return;
    p->head = 0;
    p->tail = 0;
    p->count = 0;
}

bool pipe_write(pipe_t* p, const char* data, size_t len) {
    if (p == 0 || data == 0) return false;

    for (size_t i = 0; i < len; i++) {
        if (p->count >= PIPE_BUFFER_SIZE) {
            return false; /* Pipe full */
        }
        p->buffer[p->head] = data[i];
        p->head = (p->head + 1) % PIPE_BUFFER_SIZE;
        p->count++;
    }
    return true;
}

size_t pipe_read(pipe_t* p, char* out_buf, size_t max_len) {
    if (p == 0 || out_buf == 0 || p->count == 0) return 0;

    size_t read_bytes = 0;
    while (read_bytes < max_len && p->count > 0) {
        out_buf[read_bytes] = p->buffer[p->tail];
        p->tail = (p->tail + 1) % PIPE_BUFFER_SIZE;
        p->count--;
        read_bytes++;
    }
    out_buf[read_bytes] = '\0';
    return read_bytes;
}

void sem_init(semaphore_t* sem, int32_t initial_value) {
    if (sem == 0) return;
    sem->value = initial_value;
    sem->locked = (initial_value <= 0);
}

void sem_wait(semaphore_t* sem) {
    if (sem == 0) return;
    while (sem->value <= 0) {
        __asm__ volatile ("hlt");
    }
    sem->value--;
    if (sem->value == 0) sem->locked = true;
}

void sem_signal(semaphore_t* sem) {
    if (sem == 0) return;
    sem->value++;
    sem->locked = false;
}
