/**
 * BareMetal OS - Simple Shell
 * 
 * A basic command-line shell for user space.
 */

#include <stdint.h>

/* Shell buffer */
static char cmd_buffer[256];
static int cmd_pos = 0;

/* Command definitions */
typedef struct {
    const char* name;
    const char* description;
    void (*func)(int argc, char** argv);
} command_t;

/* Forward declarations */
void cmd_help(int argc, char** argv);
void cmd_echo(int argc, char** argv);
void cmd_clear(int argc, char** argv);
void cmd_version(int argc, char** argv);
void cmd_memory(int argc, char** argv);
void cmd_date(int argc, char** argv);
void cmd_reboot(int argc, char** argv);

/* Command table */
static command_t commands[] = {
    {"help",    "Show this help message",           cmd_help},
    {"echo",    "Print text to screen",              cmd_echo},
    {"clear",   "Clear the screen",                  cmd_clear},
    {"version", "Show system version",              cmd_version},
    {"mem",     "Show memory information",           cmd_memory},
    {"date",    "Show current date and time",        cmd_date},
    {"reboot",  "Reboot the system",                 cmd_reboot},
    {NULL,      NULL,                                NULL}
};

/* Write character (stub - would call kernel) */
void putchar(char c) {
    volatile char* vga = (char*)0xB8000;
    static int cursor = 0;
    
    if (c == '\n') {
        cursor = (cursor / 80 + 1) * 80;
    } else {
        vga[cursor * 2] = c;
        vga[cursor * 2 + 1] = 0x07;
        cursor++;
    }
}

/* Write string */
void puts(const char* str) {
    while (*str) {
        putchar(*str++);
    }
}

/* Read character (stub - would call kernel) */
char getchar(void) {
    /* In real implementation, this would wait for keyboard input */
    return 0;
}

/* Command implementations */
void cmd_help(int argc, char** argv) {
    puts("Available commands:\n\n");
    
    for (int i = 0; commands[i].name != NULL; i++) {
        puts("  ");
        puts(commands[i].name);
        puts(" - ");
        puts(commands[i].description);
        puts("\n");
    }
}

void cmd_echo(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        puts(argv[i]);
        if (i < argc - 1) putchar(' ');
    }
    putchar('\n');
}

void cmd_clear(int argc, char** argv) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = 0x0F00;  /* White on black, space */
    }
    puts("\033[2J\033[H");  /* ANSI clear */
}

void cmd_version(int argc, char** argv) {
    puts("BareMetal OS v0.1.0\n");
    puts("Built from scratch with love.\n");
}

void cmd_memory(int argc, char** argv) {
    puts("Memory Information:\n");
    puts("  Total:      128 MB\n");
    puts("  Used:       16 MB\n");
    puts("  Free:       112 MB\n");
    puts("  Kernel:     0x100000 - 0x200000\n");
}

void cmd_date(int argc, char** argv) {
    puts("Current Date: 2026-07-21\n");
    puts("Current Time: 00:00:00\n");
}

void cmd_reboot(int argc, char** argv) {
    puts("Rebooting...\n");
    
    /* Triple fault to reboot */
    __asm__ volatile ("int $0x03");
    __asm__ volatile ("int $0x04");
    __asm__ volatile ("jmp $");
}

/* Parse command line */
int parse_command(char* input, char** argv, int max_args) {
    int argc = 0;
    int in_arg = 0;
    
    while (*input && argc < max_args - 1) {
        if (*input == ' ' || *input == '\t' || *input == '\n' || *input == '\r') {
            if (in_arg) {
                *input = '\0';
                in_arg = 0;
            }
        } else {
            if (!in_arg) {
                argv[argc++] = input;
                in_arg = 1;
            }
        }
        input++;
    }
    
    argv[argc] = NULL;
    return argc;
}

/* Find command */
command_t* find_command(const char* name) {
    for (int i = 0; commands[i].name != NULL; i++) {
        /* Simple string comparison */
        const char* c1 = commands[i].name;
        const char* c2 = name;
        int match = 1;
        
        while (*c1 && *c2) {
            if (*c1 != *c2) {
                match = 0;
                break;
            }
            c1++;
            c2++;
        }
        
        if (match && *c1 == *c2) {
            return &commands[i];
        }
    }
    return NULL;
}

/* Main shell loop */
void shell_main(void) {
    puts("BareMetal OS Shell v0.1.0\n");
    puts("Type 'help' for available commands.\n");
    puts("\n");
    
    while (1) {
        puts("root@baremetal:~$ ");
        
        /* Read command */
        cmd_pos = 0;
        cmd_buffer[0] = '\0';
        
        /* Read characters until newline */
        while (1) {
            char c = getchar();
            
            if (c == '\n' || c == '\r') {
                putchar('\n');
                break;
            } else if (c == '\b' || c == 127) {
                if (cmd_pos > 0) {
                    cmd_pos--;
                    putchar('\b');
                    putchar(' ');
                    putchar('\b');
                }
            } else if (c >= 32 && cmd_pos < 255) {
                cmd_buffer[cmd_pos++] = c;
                putchar(c);
            }
        }
        
        cmd_buffer[cmd_pos] = '\0';
        
        /* Skip empty commands */
        if (cmd_pos == 0) continue;
        
        /* Parse command */
        char* argv[16];
        int argc = parse_command(cmd_buffer, argv, 16);
        
        if (argc == 0) continue;
        
        /* Find and execute command */
        command_t* cmd = find_command(argv[0]);
        
        if (cmd) {
            cmd->func(argc, argv);
        } else {
            puts("Unknown command: ");
            puts(argv[0]);
            puts("\n");
            puts("Type 'help' for available commands.\n");
        }
    }
}

/* Entry point */
void _start(void) {
    shell_main();
    
    /* Should never return */
    while (1) {
        __asm__ volatile ("hlt");
    }
}
