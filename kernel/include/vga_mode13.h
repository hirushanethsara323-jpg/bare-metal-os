#ifndef VGA_MODE13_H
#define VGA_MODE13_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define VGA13_WIDTH  320
#define VGA13_HEIGHT 200
#define VGA13_MEMORY ((uint8_t*)0xA0000)

/* Common Palette Colors */
#define COLOR13_BLACK       0x00
#define COLOR13_BLUE        0x01
#define COLOR13_GREEN       0x02
#define COLOR13_CYAN        0x03
#define COLOR13_RED         0x04
#define COLOR13_MAGENTA     0x05
#define COLOR13_BROWN       0x06
#define COLOR13_LIGHT_GREY  0x07
#define COLOR13_DARK_GREY   0x08
#define COLOR13_LIGHT_BLUE  0x09
#define COLOR13_LIGHT_GREEN 0x0A
#define COLOR13_LIGHT_CYAN  0x0B
#define COLOR13_LIGHT_RED   0x0C
#define COLOR13_PINK        0x0D
#define COLOR13_YELLOW      0x0E
#define COLOR13_WHITE       0x0F

/* Function Declarations */
void vga13_clear(uint8_t color);
void vga13_draw_pixel(int x, int y, uint8_t color);
void vga13_draw_rect(int x, int y, int width, int height, uint8_t color);
void vga13_draw_line(int x0, int y0, int x1, int y1, uint8_t color);
void vga13_draw_char(int x, int y, char c, uint8_t color);
void vga13_draw_string(int x, int y, const char* str, uint8_t color);
void vga13_draw_gui_window(int x, int y, int w, int h, const char* title);
void vga13_draw_cursor(int mouse_x, int mouse_y);

#endif /* VGA_MODE13_H */
