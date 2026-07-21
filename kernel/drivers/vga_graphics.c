/**
 * Nothing OS - VGA Graphics & UI Theme Engine
 * 
 * Provides UI box window drawing, progress bar rendering, bottom status bar,
 * and dynamic color themes (Cyberpunk, Matrix Green, Ocean Blue, Retro Amber).
 */

#include "../include/vga_graphics.h"

#define VGA_MEMORY (uint16_t*)0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

extern void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

/**
 * Draw a decorative UI Window Box with Title Bar.
 */
void vga_draw_window(size_t x, size_t y, size_t width, size_t height, uint8_t color_attr, const char* title) {
    if (x + width > VGA_WIDTH || y + height > VGA_HEIGHT) return;

    /* Draw Top Border */
    terminal_putentryat('+', color_attr, x, y);
    for (size_t i = 1; i < width - 1; i++) {
        terminal_putentryat('-', color_attr, x + i, y);
    }
    terminal_putentryat('+', color_attr, x + width - 1, y);

    /* Draw Title if provided */
    if (title != NULL) {
        size_t title_len = 0;
        while (title[title_len]) title_len++;
        if (title_len < width - 4) {
            size_t title_x = x + 2;
            terminal_putentryat('[', color_attr, title_x - 1, y);
            for (size_t i = 0; i < title_len; i++) {
                terminal_putentryat(title[i], color_attr, title_x + i, y);
            }
            terminal_putentryat(']', color_attr, title_x + title_len, y);
        }
    }

    /* Draw Side Borders and Fill Background */
    for (size_t row = 1; row < height - 1; row++) {
        terminal_putentryat('|', color_attr, x, y + row);
        for (size_t col = 1; col < width - 1; col++) {
            terminal_putentryat(' ', color_attr, x + col, y + row);
        }
        terminal_putentryat('|', color_attr, x + width - 1, y + row);
    }

    /* Draw Bottom Border */
    terminal_putentryat('+', color_attr, x, y + height - 1);
    for (size_t i = 1; i < width - 1; i++) {
        terminal_putentryat('-', color_attr, x + i, y + height - 1);
    }
    terminal_putentryat('+', color_attr, x + width - 1, y + height - 1);
}

/**
 * Render a progress bar component.
 */
void vga_draw_progress_bar(size_t x, size_t y, size_t width, uint32_t percent, uint8_t color_attr) {
    if (percent > 100) percent = 100;
    if (width < 5) return;

    size_t bar_width = width - 2;
    size_t filled_count = (bar_width * percent) / 100;

    terminal_putentryat('[', color_attr, x, y);
    for (size_t i = 0; i < bar_width; i++) {
        if (i < filled_count) {
            terminal_putentryat('=', color_attr, x + 1 + i, y);
        } else {
            terminal_putentryat('-', color_attr, x + 1 + i, y);
        }
    }
    terminal_putentryat(']', color_attr, x + width - 1, y);
}

/**
 * Draw bottom status bar across row 24.
 */
void vga_draw_status_bar(const char* status_text, uint8_t color_attr) {
    size_t len = 0;
    while (status_text[len]) len++;

    for (size_t x = 0; x < VGA_WIDTH; x++) {
        if (x < len) {
            terminal_putentryat(status_text[x], color_attr, x, VGA_HEIGHT - 1);
        } else {
            terminal_putentryat(' ', color_attr, x, VGA_HEIGHT - 1);
        }
    }
}

/**
 * Get Color Attribute byte for requested Theme.
 */
uint8_t vga_get_theme_color(ui_theme_t theme, bool is_title) {
    switch (theme) {
        case THEME_CYBERPUNK:
            return is_title ? (0x0D /* Light Magenta on Black */) : (0x0B /* Light Cyan on Black */);
        case THEME_MATRIX:
            return is_title ? (0x0A /* Light Green on Black */) : (0x02 /* Green on Black */);
        case THEME_OCEAN:
            return is_title ? (0x09 /* Light Blue on Black */) : (0x0B /* Light Cyan on Black */);
        case THEME_AMBER:
            return is_title ? (0x0E /* Yellow/Brown on Black */) : (0x06 /* Brown on Black */);
        case THEME_DEFAULT:
        default:
            return is_title ? (0x0F /* White on Black */) : (0x07 /* Light Grey on Black */);
    }
}

const char* vga_get_theme_name(ui_theme_t theme) {
    switch (theme) {
        case THEME_CYBERPUNK: return "Cyberpunk Pink & Cyan";
        case THEME_MATRIX:    return "Matrix Neon Green";
        case THEME_OCEAN:     return "Oceanic Blue";
        case THEME_AMBER:     return "Retro CRT Amber";
        case THEME_DEFAULT:
        default:              return "Classic Monochrome";
    }
}
