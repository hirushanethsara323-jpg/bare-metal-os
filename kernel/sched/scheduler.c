/**
 * BareMetal OS - Process Scheduler
 * 
 * Implements round-robin preemptive multitasking.
 */

#include <stdint.h>

/* Maximum processes */
#define MAX_PROCESSES 64

/* Process states */
typedef enum {
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_READY,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_SLEEPING,
    PROCESS_STATE_ZOMBIE,
    PROCESS_STATE_DEAD
} process_state_t;

/* Process control block (PCB) */
typedef struct process {
    uint32_t pid;                    /* Process ID */
    uint32_t ppid;                   /* Parent PID */
    char name[32];                   /* Process name */
    process_state_t state;          /* Current state */
    uint32_t priority;               /* Scheduling priority */
    uint32_t kernel_stack;           /* Kernel stack pointer */
    uint32_t user_stack;             /* User stack pointer */
    uint32_t registers[16];          /* CPU registers (context) */
    uint32_t program_counter;        /* Instruction pointer */
    uint32_t flags;                   /* Process flags */
    uint32_t exit_code;              /* Exit status */
    uint32_t sleep_until;            /* Wake time for sleeping */
    uint32_t time_used;              /* CPU time used */
    uint32_t wake_time;              /* Time when process should wake */
    struct process* next;            /* Next process in queue */
    struct process* parent;          /* Parent process */
} process_t;

/* Scheduler state */
static process_t* processes[MAX_PROCESSES];
static process_t* current_process = NULL;
static process_t* ready_queue = NULL;
static uint32_t next_pid = 1;
static uint32_t running_processes = 0;
static uint32_t schedule_count = 0;

/* Initialize scheduler */
void sched_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i] = NULL;
    }
    
    ready_queue = NULL;
    current_process = NULL;
    next_pid = 1;
    running_processes = 0;
    schedule_count = 0;
}

/* Create a new process */
int sched_create_process(void (*entry)(void), const char* name, uint32_t priority) {
    if (running_processes >= MAX_PROCESSES) {
        return -1;  /* No slots available */
    }
    
    /* Find free slot */
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i] == NULL) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) return -1;
    
    /* Allocate process structure */
    process_t* proc = (process_t*)0x50000;  /* Kernel memory for PCBs */
    proc = (process_t*)((uint32_t)proc + (slot * sizeof(process_t)));
    
    /* Initialize PCB */
    proc->pid = next_pid++;
    proc->ppid = current_process ? current_process->pid : 0;
    proc->state = PROCESS_STATE_READY;
    proc->priority = priority;
    proc->kernel_stack = 0x90000 - (slot * 0x1000);  /* Stack per process */
    proc->user_stack = 0xBFFF0000 - (slot * 0x1000);
    proc->time_used = 0;
    proc->exit_code = 0;
    proc->sleep_until = 0;
    proc->parent = current_process;
    proc->next = NULL;
    
    /* Copy name safely */
    int i = 0;
    while (name[i] && i < 31) {
        proc->name[i] = name[i];
        i++;
    }
    proc->name[i] = '\0';
    
    /* Initialize registers (for first run) */
    proc->registers[0] = 0;   /* EAX */
    proc->registers[1] = 0;   /* EBX */
    proc->registers[2] = 0;   /* ECX */
    proc->registers[3] = 0;   /* EDX */
    proc->registers[4] = 0;   /* ESI */
    proc->registers[5] = 0;   /* EDI */
    proc->registers[6] = 0;   /* EBP */
    proc->registers[7] = 0;   /* ESP (will be set to user stack) */
    proc->registers[8] = 0;   /* EIP */
    
    /* Store entry point */
    proc->program_counter = (uint32_t)entry;
    
    /* Add to ready queue */
    if (ready_queue == NULL) {
        ready_queue = proc;
    } else {
        process_t* tail = ready_queue;
        while (tail->next) tail = tail->next;
        tail->next = proc;
    }
    
    processes[slot] = proc;
    running_processes++;
    
    return proc->pid;
}

/* Schedule - called by timer interrupt */
void sched_schedule(void) {
    schedule_count++;
    
    if (!ready_queue) return;
    if (ready_queue->state == PROCESS_STATE_READY) {
        current_process = ready_queue;
    }
    
    /* Move current to back of queue (round-robin) */
    if (ready_queue->next) {
        process_t* first = ready_queue;
        process_t* last = first;
        
        while (last->next) last = last->next;
        
        ready_queue = first->next;
        first->next = NULL;
        last->next = first;
    }
    
    /* Context switch would happen here */
}

/* Yield CPU to other processes */
void sched_yield(void) {
    if (current_process) {
        /* Put current process back in queue */
        current_process->state = PROCESS_STATE_READY;
    }
    sched_schedule();
}

/* Exit current process */
void sched_exit(int exit_code) {
    if (current_process) {
        current_process->state = PROCESS_STATE_DEAD;
        current_process->exit_code = exit_code;
    }
    sched_schedule();
}

/* Sleep for milliseconds */
void sched_sleep(uint32_t ms) {
    if (current_process) {
        current_process->state = PROCESS_STATE_SLEEPING;
        current_process->sleep_until = schedule_count + (ms / 10);  /* Assuming 10ms tick */
    }
    sched_schedule();
}

/* Get current process ID */
uint32_t sched_getpid(void) {
    return current_process ? current_process->pid : 0;
}

/* Get scheduler statistics */
void sched_get_stats(uint32_t* procs, uint32_t* schedules) {
    if (procs) *procs = running_processes;
    if (schedules) *schedules = schedule_count;
}
