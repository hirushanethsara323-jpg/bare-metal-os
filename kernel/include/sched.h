#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_PROCESSES 16
#define STACK_SIZE    4096

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

/* Process Control Block (PCB) Structure */
typedef struct process {
    uint32_t pid;
    char name[32];
    process_state_t state;
    uint32_t esp;                   /* Saved Stack Pointer */
    uint32_t ebp;                   /* Saved Base Pointer */
    uint32_t eip;                   /* Instruction Pointer */
    uint8_t* stack;                 /* Stack Buffer */
    struct process* next;
} process_t;

/* Function Declarations */
void scheduler_init(void);
process_t* process_create(const char* name, void (*entry_point)(void));
void process_yield(void);
void process_kill(uint32_t pid);
process_t* process_get_current(void);
process_t* process_get_list(void);

#endif /* SCHED_H */
