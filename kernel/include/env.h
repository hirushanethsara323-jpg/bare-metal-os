#ifndef ENV_H
#define ENV_H

#include <stdint.h>
#include <stdbool.h>

/* Environment Variable Entry Node */
typedef struct env_var {
    char key[32];
    char value[128];
    struct env_var* next;
} env_var_t;

/* Function Declarations */
void env_init(void);
bool ksetenv(const char* key, const char* val);
const char* kgetenv(const char* key);
env_var_t* env_get_list(void);

#endif /* ENV_H */
