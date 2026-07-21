/**
 * Nothing OS - Real Time Clock (RTC / CMOS) Driver
 * 
 * Reads CMOS registers for wall-clock time and calendar information.
 */

#include "../include/rtc.h"
#include "../include/io.h"

static uint8_t get_cmos_register(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static int cmos_update_in_progress(void) {
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

/**
 * Read current Date & Time from RTC CMOS chips.
 * Handles BCD to binary conversion.
 */
void rtc_read_time(rtc_time_t* time) {
    if (time == 0) return;

    /* Wait if CMOS clock update is in progress */
    while (cmos_update_in_progress());

    uint8_t sec   = get_cmos_register(0x00);
    uint8_t min   = get_cmos_register(0x02);
    uint8_t hour  = get_cmos_register(0x04);
    uint8_t day   = get_cmos_register(0x07);
    uint8_t month = get_cmos_register(0x08);
    uint8_t year  = get_cmos_register(0x09);
    uint8_t reg_b = get_cmos_register(0x0B);

    /* Convert BCD to Binary if necessary */
    if (!(reg_b & 0x04)) {
        sec   = (sec & 0x0F) + ((sec / 16) * 10);
        min   = (min & 0x0F) + ((min / 16) * 10);
        hour  = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
        day   = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year  = (year & 0x0F) + ((year / 16) * 10);
    }

    /* Convert 12-hour clock to 24-hour clock if needed */
    if (!(reg_b & 0x02) && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    time->second = sec;
    time->minute = min;
    time->hour   = hour;
    time->day    = day;
    time->month  = month;
    time->year   = 2000 + year;
}
