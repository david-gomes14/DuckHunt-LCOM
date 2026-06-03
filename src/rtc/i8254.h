#ifndef _I8254_H
#define _I8254_H

/* IRQ line */
#define RTC_IRQ 8

/* RTC ports */
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

/* RTC registers */
#define RTC_REG_DAY   0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR  0x09

#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B

#define RTC_UIP_MSK BIT(7)
#define RTC_DM_MSK  BIT(2)

#endif
