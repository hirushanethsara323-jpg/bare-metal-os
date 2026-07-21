/**
 * Nothing OS - Lightweight Embedded C-Script Runtime Engine
 * 
 * Lead: CScript Runtime Lead
 */

#ifndef CSCRIPT_H
#define CSCRIPT_H

#include <stdint.h>
#include <stdbool.h>

void cscript_init(void);
void cscript_eval(const char* script_str);

#endif /* CSCRIPT_H */
