#ifndef VT_H
#define VT_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_VIRTUAL_TERMINALS 4

/* Virtual Terminal Console Structure */
typedef struct {
    uint8_t id;
    uint16_t screen_buffer[80 * 25];
    size_t cursor_x;
    size_t cursor_y;
    uint8_t color_attr;
    bool active;
} vterminal_t;

/* Function Declarations */
void vt_init(void);
void vt_switch(uint8_t id);
uint8_t vt_get_active_id(void);

#endif /* VT_H */
