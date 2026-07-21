#ifndef RTC_H
#define RTC_H

#include <stdint.h>

/* CMOS Ports */
#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

/* System Date & Time Structure */
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint32_t year;
} rtc_time_t;

/* Functions */
void rtc_read_time(rtc_time_t* time);

#endif /* RTC_H */
