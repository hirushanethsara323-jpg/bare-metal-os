/**
 * Nothing OS - Operating Systems Academic Theory & Pedagogy Engine
 * 
 * Managed by Prof. Systems Research Chair (Chief Academic Officer)
 */

#ifndef PROF_H
#define PROF_H

#include <stdint.h>
#include <stdbool.h>

void prof_init(void);
void prof_print_lecture(const char* topic);
void prof_verify_theory(void);

#endif /* PROF_H */
