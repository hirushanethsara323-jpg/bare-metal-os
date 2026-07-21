/**
 * Nothing OS - VESA VBE High-Resolution 32-bit TrueColor Framebuffer Driver
 * 
 * Provides 1024x768 32-bit ARGB Linear Framebuffer rendering, high-res GUI
 * desktop background, window drop shadows, and true-color window framing.
 */

#include "../include/vbe.h"
#include "../include/serial.h"

static uint32_t* vbe_lfb = (uint32_t*)0xFD000000; /* Simulated/Physical LFB Default */
static uint16_t  vbe_width = 1024;
static uint16_t  vbe_height = 768;
static uint8_t   vbe_bpp = 32;

void vbe_init(uint32_t lfb_addr, uint16_t width, uint16_t height, uint8_t bpp) {
    if (lfb_addr != 0) {
        vbe_lfb = (uint32_t*)(uintptr_t)lfb_addr;
    }
    vbe_width = width;
    vbe_height = height;
    vbe_bpp = bpp;

    klog(KLOG_INFO, "VESA VBE 1024x768 32-bit ARGB TrueColor Framebuffer Initialized.");
}

void vbe_put_pixel_32(uint16_t x, uint16_t y, uint32_t argb) {
    if (x < vbe_width && y < vbe_height) {
        vbe_lfb[y * vbe_width + x] = argb;
    }
}

void vbe_draw_rect_32(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t argb) {
    for (uint16_t r = 0; r < h; r++) {
        for (uint16_t c = 0; c < w; c++) {
            vbe_put_pixel_32(x + c, y + r, argb);
        }
    }
}

/**
 * Render High-Definition 1024x768 True-Color Desktop Interface.
 */
void vbe_render_hd_gui(void) {
    /* Solid Dark Slate Background (0xFF1E222A) */
    vbe_draw_rect_32(0, 0, vbe_width, vbe_height, 0xFF1E222A);

    /* Top HD Status Bar (Gradient Cyan Blue 0xFF0F3460) */
    vbe_draw_rect_32(0, 0, vbe_width, 32, 0xFF0F3460);

    /* Main Window Frame Drop Shadow (0xFF111111) */
    vbe_draw_rect_32(108, 108, 800, 550, 0xFF111111);

    /* Main Window Body (0xFF222831) */
    vbe_draw_rect_32(100, 100, 800, 550, 0xFF222831);

    /* Title Bar Header (0xFF00ADB5) */
    vbe_draw_rect_32(100, 100, 800, 40, 0xFF00ADB5);

    /* Close Button (0xFFE84545) */
    vbe_draw_rect_32(870, 108, 24, 24, 0xFFE84545);

    klog(KLOG_INFO, "Rendered HD 1024x768 32-bit VESA TrueColor GUI Window Server Frame.");
}
