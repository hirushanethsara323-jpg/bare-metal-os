#ifndef IPC_H
#define IPC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PIPE_BUFFER_SIZE 64

/* IPC Ring Buffer Pipe Structure */
typedef struct {
    char buffer[PIPE_BUFFER_SIZE];
    size_t head;
    size_t tail;
    size_t count;
} pipe_t;

/* Semaphore Structure */
typedef struct {
    int32_t value;
    bool locked;
} semaphore_t;

/* Function Declarations */
void ipc_init(void);
void pipe_init(pipe_t* p);
bool pipe_write(pipe_t* p, const char* data, size_t len);
size_t pipe_read(pipe_t* p, char* out_buf, size_t max_len);

void sem_init(semaphore_t* sem, int32_t initial_value);
void sem_wait(semaphore_t* sem);
void sem_signal(semaphore_t* sem);

#endif /* IPC_H */
