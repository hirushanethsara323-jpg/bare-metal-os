#ifndef VGA_GRAPHICS_H
#define VGA_GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Theme Modes */
typedef enum {
    THEME_DEFAULT,
    THEME_CYBERPUNK,
    THEME_MATRIX,
    THEME_OCEAN,
    THEME_AMBER
} ui_theme_t;

/* Function Declarations */
void vga_draw_window(size_t x, size_t y, size_t width, size_t height, uint8_t color_attr, const char* title);
void vga_draw_progress_bar(size_t x, size_t y, size_t width, uint32_t percent, uint8_t color_attr);
void vga_draw_status_bar(const char* status_text, uint8_t color_attr);
uint8_t vga_get_theme_color(ui_theme_t theme, bool is_title);
const char* vga_get_theme_name(ui_theme_t theme);

#endif /* VGA_GRAPHICS_H */
