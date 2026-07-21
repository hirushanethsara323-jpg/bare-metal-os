#ifndef VBE_H
#define VBE_H

#include <stdint.h>
#include <stdbool.h>

#define VBE_1024x768_32BPP 0x0118

/* VBE Mode Info Structure */
typedef struct {
    uint16_t mode_attributes;
    uint8_t  win_a_attributes;
    uint8_t  win_b_attributes;
    uint16_t win_granularity;
    uint16_t win_size;
    uint16_t win_a_segment;
    uint16_t win_b_segment;
    uint32_t win_func_ptr;
    uint16_t bytes_per_scan_line;

    uint16_t x_resolution;
    uint16_t y_resolution;
    uint8_t  x_char_size;
    uint8_t  y_char_size;
    uint8_t  number_of_planes;
    uint8_t  bits_per_pixel;
    uint8_t  number_of_banks;
    uint8_t  memory_model;
    uint8_t  bank_size;
    uint8_t  number_of_image_pages;
    uint8_t  reserved1;

    uint8_t  red_mask_size;
    uint8_t  red_field_position;
    uint8_t  green_mask_size;
    uint8_t  green_field_position;
    uint8_t  blue_mask_size;
    uint8_t  blue_field_position;
    uint8_t  rsvd_mask_size;
    uint8_t  rsvd_field_position;
    uint8_t  direct_color_mode_info;

    uint32_t phys_base_ptr; /* Linear Framebuffer Base Pointer */
    uint32_t offscreen_mem_offset;
    uint16_t offscreen_mem_size;
} __attribute__((packed)) vbe_mode_info_t;

/* Function Declarations */
void vbe_init(uint32_t lfb_addr, uint16_t width, uint16_t height, uint8_t bpp);
void vbe_put_pixel_32(uint16_t x, uint16_t y, uint32_t argb);
void vbe_draw_rect_32(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t argb);
void vbe_render_hd_gui(void);

#endif /* VBE_H */
