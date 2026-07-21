/**
 * Nothing OS - Process Scheduler & Context Switching Engine
 * 
 * Implements Process Control Blocks (PCBs), process creation, Round-Robin
 * scheduling, process yielding, process termination, and task listing.
 */

#include "../include/sched.h"
#include "../include/heap.h"
#include "../include/serial.h"

static process_t* process_list = NULL;
static process_t* current_process = NULL;
static uint32_t next_pid = 1;

static void str_copy(char* dest, const char* src, size_t max) {
    size_t i = 0;
    while (src[i] && i < max - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

/**
 * Worker stub tasks.
 */
static void idle_task(void) {
    while (1) {
        __asm__ volatile ("hlt");
    }
}

static void sysmon_task(void) {
    while (1) {
        klog(KLOG_INFO, "System Health Monitor Worker Pulse Active.");
        process_yield();
    }
}

/**
 * Initialize Process Scheduler and spawn baseline kernel tasks.
 */
void scheduler_init(void) {
    process_list = NULL;
    current_process = NULL;
    next_pid = 1;

    /* PID 1: Main Kernel Task */
    process_t* main_task = (process_t*)kmalloc(sizeof(process_t));
    if (main_task != NULL) {
        main_task->pid = next_pid++;
        str_copy(main_task->name, "kernel_main", 32);
        main_task->state = PROCESS_RUNNING;
        main_task->stack = NULL;
        main_task->next = NULL;
        process_list = main_task;
        current_process = main_task;
    }

    /* Spawn background System Monitor Task */
    process_create("sysmon_daemon", sysmon_task);

    /* Spawn background Idle Task */
    process_create("idle_worker", idle_task);
}

/**
 * Create and register a new kernel task process.
 */
process_t* process_create(const char* name, void (*entry_point)(void)) {
    process_t* new_proc = (process_t*)kmalloc(sizeof(process_t));
    if (new_proc == NULL) {
        return NULL;
    }

    new_proc->pid = next_pid++;
    str_copy(new_proc->name, name, 32);
    new_proc->state = PROCESS_READY;
    new_proc->stack = (uint8_t*)kmalloc(STACK_SIZE);
    new_proc->eip = (uint32_t)entry_point;
    
    if (new_proc->stack != NULL) {
        new_proc->esp = (uint32_t)(new_proc->stack + STACK_SIZE - 16);
        new_proc->ebp = new_proc->esp;
    } else {
        new_proc->esp = 0;
        new_proc->ebp = 0;
    }

    /* Append to process list */
    new_proc->next = process_list;
    process_list = new_proc;

    return new_proc;
}

/**
 * Yield CPU time to next ready process (Round-Robin).
 */
void process_yield(void) {
    if (current_process == NULL || current_process->next == NULL) {
        if (process_list != NULL) {
            current_process = process_list;
        }
        return;
    }

    /* Save current state */
    if (current_process->state == PROCESS_RUNNING) {
        current_process->state = PROCESS_READY;
    }

    /* Select next ready task */
    process_t* next_proc = current_process->next;
    while (next_proc != NULL && next_proc->state != PROCESS_READY) {
        next_proc = next_proc->next;
    }

    if (next_proc == NULL) {
        /* Wrap around to list head */
        next_proc = process_list;
        while (next_proc != NULL && next_proc->state != PROCESS_READY) {
            next_proc = next_proc->next;
        }
    }

    if (next_proc != NULL) {
        current_process = next_proc;
        current_process->state = PROCESS_RUNNING;
    }
}

/**
 * Terminate process by PID.
 */
void process_kill(uint32_t pid) {
    process_t* curr = process_list;
    while (curr != NULL) {
        if (curr->pid == pid && curr->pid != 1 /* Do not kill kernel init */) {
            curr->state = PROCESS_TERMINATED;
            klog(KLOG_INFO, "Process Terminated.");
            return;
        }
        curr = curr->next;
    }
}

process_t* process_get_current(void) {
    return current_process;
}

process_t* process_get_list(void) {
    return process_list;
}
