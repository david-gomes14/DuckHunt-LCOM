#include <lcom/lcf.h>

#include <minix/syslib.h>
#include <minix/sysutil.h>

#include "rtc.h"
#include <stdbool.h>   


int bcd_to_bin(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}


int rtc_read_reg(uint8_t reg, uint8_t *val){
	int r = sys_outb(RTC_ADDR_REG, reg);
	if(r != 0) return r;
	uint32_t tmp = 0;
	r = sys_inb(RTC_DATA_REG, &tmp);
	if(r != 0) return r;
	*val = tmp & 0xFF;
	return 0;
}


int rtc_read_date(rtc_date_t *date) {
	uint8_t regA = 0;
	int r = 0;
	do{
		r = rtc_read_reg(RTC_REG_A, &regA);
		if(r != 0) return r;
		if(regA & RTC_UIP_MSK) tickdelay(micros_to_ticks(200));
	} while (regA & RTC_UIP_MSK);
	uint8_t regB = 0;
	r = rtc_read_reg(RTC_REG_B, &regB);
	if (r != 0) return r;
	bool binary = (regB & RTC_DM_MSK) != 0;
	uint8_t hours, minutes, day, month, year;
	r = rtc_read_reg(RTC_REG_HOURS,   &hours);   if (r != 0) return r;
	r = rtc_read_reg(RTC_REG_MINUTES, &minutes); if (r != 0) return r;
	r = rtc_read_reg(RTC_REG_DAY,     &day);     if (r != 0) return r;
	r = rtc_read_reg(RTC_REG_MONTH,   &month);   if (r != 0) return r;
	r = rtc_read_reg(RTC_REG_YEAR,    &year);    if (r != 0) return r;
	if (!binary) {
		hours   = bcd_to_bin(hours);
		minutes = bcd_to_bin(minutes);
		day     = bcd_to_bin(day);
		month   = bcd_to_bin(month);
		year    = bcd_to_bin(year);
	}
	date->hour   = hours;
	date->minute = minutes;
	date->day    = day;
	date->month  = month;
	date->year   = 2000 + year;
	return 0;
}
