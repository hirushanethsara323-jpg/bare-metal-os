#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stdbool.h>

/* Mouse State Structure */
typedef struct {
    int32_t x;
    int32_t y;
    bool left_button;
    bool right_button;
    bool middle_button;
} mouse_state_t;

/* Function Declarations */
void mouse_init(void);
void mouse_handle_byte(uint8_t b);
void mouse_get_state(mouse_state_t* state);

#endif /* MOUSE_H */
