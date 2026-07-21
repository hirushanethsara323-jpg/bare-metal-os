/**
 * BareMetal OS - Standard Integer Types
 * 
 * Provides standard integer type definitions.
 */

#ifndef __STDINT_H
#define __STDINT_H

/* Signed integers */
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* Unsigned integers */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/* Pointer types */
typedef unsigned long uintptr_t;
typedef signed long intptr_t;

/* Size types */
typedef unsigned long size_t;
typedef signed long ssize_t;

/* Limit definitions */
#define INT8_MIN   (-128)
#define INT8_MAX   127
#define INT16_MIN  (-32768)
#define INT16_MAX  32767
#define INT32_MIN  (-2147483647 - 1)
#define INT32_MAX  2147483647
#define INT64_MIN  (-9223372036854775807LL - 1)
#define INT64_MAX  9223372036854775807LL

#define UINT8_MAX  255
#define UINT16_MAX 65535
#define UINT32_MAX 4294967295U
#define UINT64_MAX 18446744073709551615ULL

#define SIZE_MAX   UINT32_MAX

/* NULL pointer */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* Boolean type */
#ifndef __cplusplus
typedef int bool;
#define true 1
#define false 0
#endif

#endif /* __STDINT_H */
