#ifndef STDDEF_H
#define STDDEF_H

#include <stdint.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef unsigned long size_t;
typedef long ptrdiff_t;

#define offsetof(type, member) __builtin_offsetof(type, member)

#endif /* STDDEF_H */
