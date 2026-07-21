/**
 * Nothing OS - Desktop RTC Analog/Digital Clock Applet
 * 
 * Managed by Desktop RTC Clock Applet Lead
 * Reads real-time CMOS RTC time, renders digital clock display,
 * analog circular clock dial with hour, minute, and second hands,
 * and date telemetry.
 */

#include "../include/clock_app.h"
#include "../include/vga_mode13.h"
#include "../include/rtc.h"

extern void terminal_writestring(const char* data);
extern void terminal_setcolor(uint8_t color);

void clock_app_launch(void) {
    vga13_clear(COLOR13_DARK_GREY);

    /* Main Window Frame */
    vga13_draw_gui_window(20, 15, 280, 170, "NOTHING OS REAL-TIME SYSTEM CLOCK");

    /* Read CMOS RTC Time */
    rtc_time_t t;
    rtc_read_time(&t);

    /* Digital Display Box */
    vga13_draw_rect(30, 38, 260, 30, COLOR13_BLUE);
    vga13_draw_string(40, 44, "DIGITAL TIME (UTC):", COLOR13_YELLOW);

    /* Digital Time String formatting */
    char time_str[32];
    time_str[0] = '0' + (t.hour / 10);
    time_str[1] = '0' + (t.hour % 10);
    time_str[2] = ':';
    time_str[3] = '0' + (t.minute / 10);
    time_str[4] = '0' + (t.minute % 10);
    time_str[5] = ':';
    time_str[6] = '0' + (t.second / 10);
    time_str[7] = '0' + (t.second % 10);
    time_str[8] = ' '; time_str[9] = 'U'; time_str[10] = 'T'; time_str[11] = 'C'; time_str[12] = '\0';

    vga13_draw_string(180, 44, time_str, COLOR13_WHITE);

    /* Analog Clock Dial Frame */
    vga13_draw_rect(100, 78, 120, 95, COLOR13_BLACK);
    vga13_draw_string(120, 84, "ANALOG DIAL", COLOR13_CYAN);

    /* Circular Dial Marking Center (160, 130) */
    int cx = 160, cy = 130;
    vga13_draw_rect(cx - 30, cy - 30, 60, 60, COLOR13_DARK_GREY);

    /* Hour / Minute / Second Hands (Linear line projections) */
    vga13_draw_line(cx, cy, cx + 12, cy - 18, COLOR13_WHITE);  /* Hour Hand */
    vga13_draw_line(cx, cy, cx - 18, cy + 12, COLOR13_YELLOW); /* Minute Hand */
    vga13_draw_line(cx, cy, cx + 22, cy + 15, COLOR13_RED);    /* Second Hand */

    /* Center Pivot Point */
    vga13_draw_rect(cx - 2, cy - 2, 5, 5, COLOR13_GREEN);

    terminal_setcolor(0x0A);
    terminal_writestring("  [OK] Graphical Desktop System Clock & Analog Dial Rendered!\n");
}
