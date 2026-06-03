#ifndef _I8254_H
#define _I8254_H

/** @defgroup i8254 i8254
 * @{
 * 
 * Constants for programming the i8354 RTC.
 */

#define RTC_IRQ 8 /**< @brief RTC IRQ line */

/* I/O port addresses */
#define RTC_ADDR_REG 0x70 /**< @brief RTC address register */
#define RTC_DATA_REG 0x71 /**< @brief RTC data register */

/* RTC date registers */

#define RTC_REG_DAY   0x07 /**< @brief Day of month (1–31) */
#define RTC_REG_MONTH 0x08 /**< @brief Month (1–12) */
#define RTC_REG_YEAR  0x09 /**< @brief Year (2-digit, e.g. 25 for 2025) */

/* RTC control registers */
#define RTC_REG_A 0x0A /**< @brief Register A: oscillator and update status */
#define RTC_REG_B 0x0B /**< @brief Register B: output and data mode control */

/* Register A bitmasks */
#define RTC_UIP_MSK BIT(7) /**< @brief Update In Progress flag */

/* Register B bitmasks */
#define RTC_DM_MSK BIT(2) /**< @brief Data Mode: 1 = binary, 0 = BCD */

/**@}*/

#endif /* _I8254_H */
