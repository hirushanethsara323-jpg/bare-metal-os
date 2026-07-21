#ifndef PKG_H
#define PKG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_PACKAGES 16

/* Kernel Package Entry Node */
typedef struct pkg_node {
    char name[32];
    char version[16];
    size_t binary_size;
    char sha256_hash[65];
    bool installed;
    struct pkg_node* next;
} pkg_node_t;

/* Function Declarations */
void pkg_init(void);
bool pkg_install(const char* name, const char* version, const uint8_t* binary, size_t size);
pkg_node_t* pkg_get_installed(void);

#endif /* PKG_H */
