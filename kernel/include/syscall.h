#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stdbool.h>

/* POSIX / Nothing OS System Call Numbers */
#define SYS_YIELD     0
#define SYS_WRITE     1
#define SYS_GETPID    2
#define SYS_UPTIME    3
#define SYS_ALLOC     4
#define SYS_FREE      5

#define SYSCALL_VECTOR 0x80

/* Function Declarations */
void syscall_init(void);
int32_t syscall_dispatch(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3);
int32_t syscall_invoke(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3);

#endif /* SYSCALL_H */
