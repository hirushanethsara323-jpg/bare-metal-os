/**
 * Nothing OS - Global System Environment Variables Store
 * 
 * Provides global key-value configuration variables, dynamic memory allocation
 * for custom environment entries, and system environment lookups.
 */

#include "../include/env.h"
#include "../include/heap.h"
#include "../include/serial.h"

static env_var_t* env_head = NULL;

static void str_copy(char* dest, const char* src, size_t max) {
    size_t i = 0;
    while (src[i] && i < max - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static int str_cmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/**
 * Set or update global OS system environment key-value entry.
 */
bool ksetenv(const char* key, const char* val) {
    if (key == NULL || val == NULL) return false;

    /* Check if key already exists */
    env_var_t* curr = env_head;
    while (curr != NULL) {
        if (str_cmp(curr->key, key) == 0) {
            str_copy(curr->value, val, 128);
            return true;
        }
        curr = curr->next;
    }

    /* Allocate new entry node */
    env_var_t* new_var = (env_var_t*)kmalloc(sizeof(env_var_t));
    if (new_var == NULL) return false;

    str_copy(new_var->key, key, 32);
    str_copy(new_var->value, val, 128);

    new_var->next = env_head;
    env_head = new_var;

    return true;
}

/**
 * Retrieve system environment value by key string.
 */
const char* kgetenv(const char* key) {
    if (key == NULL) return NULL;
    env_var_t* curr = env_head;
    while (curr != NULL) {
        if (str_cmp(curr->key, key) == 0) {
            return curr->value;
        }
        curr = curr->next;
    }
    return NULL;
}

env_var_t* env_get_list(void) {
    return env_head;
}

/**
 * Populate default system environment variables.
 */
void env_init(void) {
    env_head = NULL;

    ksetenv("PATH", "/bin:/usr/bin");
    ksetenv("HOME", "/root");
    ksetenv("USER", "admin");
    ksetenv("SHELL", "/bin/nothing_shell");
    ksetenv("TERM", "vga80x25");
    ksetenv("THEME", "cyberpunk");
    ksetenv("ARCH", "i386-protected");

    klog(KLOG_INFO, "System Global Environment Store Initialized.");
}
