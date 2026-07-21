#ifndef WM_H
#define WM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_GUI_WINDOWS 8

/* GUI Window Structure */
typedef struct gui_window {
    uint32_t id;
    char title[32];
    int x, y, width, height;
    bool is_active;
    bool is_minimized;
    uint32_t bg_color;
    struct gui_window* next;
} gui_window_t;

/* Function Declarations */
void wm_init(void);
gui_window_t* wm_create_window(const char* title, int x, int y, int w, int h, uint32_t bg_color);
void wm_redraw_desktop(void);
gui_window_t* wm_get_windows(void);

#endif /* WM_H */
