#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include "i8254.h"

typedef struct {
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_date_t;

int rtc_read_reg(uint8_t reg, uint8_t *val);
int rtc_read_date(rtc_date_t *date);
int bcd_to_bin(uint8_t bcd);

#endif
