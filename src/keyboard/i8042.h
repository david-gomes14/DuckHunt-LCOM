#ifndef _I8042_H_
#define _I8042_H_

#include <lcom/lcf.h>

/* IRQ line */
#define KBC_IRQ 1

/* Ports */
#define OUTPUT_BUF_REG    0x60
#define INPUT_BUF_REG     0x60
#define KBC_STATUS_REG    0x64
#define KBC_COMMAND_REG   0x64

/* Status Register bits */
#define OUTPUT_BUFFER_FULL         BIT(0) /* Output buffer full */
#define INPUT_BUFFER_FULL          BIT(1) /* Input buffer full */
#define Aux                        BIT(5) /* Mouse data */
#define TIMEOUT                    BIT(6) /* Invalid data */
#define PARITY                     BIT(7) /* Invalid data */

/* Scancodes */
#define ESC_BREAK_CODE     0x81
#define TWO_BYTE_SCANCODE  0xE0

/* Commands */
#define READ_CMD_BYTE   0x20
#define WRITE_CMD_BYTE  0x60

/* Command byte bits */
#define KBC_INT BIT(0)

/* Polling / command timing */
#define MAX_RETRIES  10

#endif

