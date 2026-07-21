#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdint.h>
#include <stdbool.h>

/* Standard POSIX Signals */
#define SIGHUP    1
#define SIGINT    2
#define SIGQUIT   3
#define SIGILL    4
#define SIGTRAP   5
#define SIGABRT   6
#define SIGBUS    7
#define SIGFPE    8
#define SIGKILL   9
#define SIGUSR1   10
#define SIGSEGV   11
#define SIGUSR2   12
#define SIGPIPE   13
#define SIGALRM   14
#define SIGTERM   15

/* Signal Handler Callback Pointer */
typedef void (*signal_handler_t)(int);

/* Function Declarations */
void signal_init(void);
bool signal_register(int sig, signal_handler_t handler);
bool signal_send(uint32_t pid, int sig);
void kernel_panic(const char* reason);

#endif /* SIGNAL_H */
