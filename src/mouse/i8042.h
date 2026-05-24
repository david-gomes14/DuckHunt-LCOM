#ifndef _I8042_H_
#define _I8042_H_

/* IRQ line */
#define KBC_IRQ 1
#define MOUSE_IRQ 12

/* KBC ports */
#define OUTPUT_BUF_REG    0x60
#define INPUT_BUF_REG     0x60
#define KBC_STATUS_REG    0x64
#define KBC_COMMAND_REG   0x64

/* Status Register bits */
#define OUTPUT_BUFFER_FULL         BIT(0) /* Output buffer full */
#define INPUT_BUFFER_FULL          BIT(1) /* Input buffer full */
#define AUX                        BIT(5) /* Mouse data */
#define TIMEOUT                    BIT(6) /* Invalid data */
#define PARITY                     BIT(7) /* Invalid data */

/* Scancodes */
#define ESC_BREAK_CODE     0x81
#define TWO_BYTE_SCANCODE  0xE0

/* KBC commands */
#define READ_CMD_BYTE   0x20
#define WRITE_CMD_BYTE  0x60
#define WRITE_TO_MOUSE  0xD4

/* Command byte bits */
#define KBC_INT BIT(0)
#define MOUSE_INT BIT(1)

/* Mouse Commands */
#define MOUSE_RESET            0xFF  /* Software reset, response is ACK */
#define MOUSE_RESEND           0xFE  /* Host detected invalid output */
#define MOUSE_DEFAULTS         0xF6  /* Reset and disables device */
#define MOUSE_DISABLE_DATA     0xF5  /* Stream Mode cannot receive data */
#define MOUSE_ENABLE_DATA      0xF4  /* Stream Mode can receive data */
#define MOUSE_READ_DATA        0xEB 
#define MOUSE_SET_SAMPLE_RATE  0xF3  
#define MOUSE_SET_RESOLUTION   0xE8  
#define MOUSE_STATUS_REQUEST   0xE9  /* ACK followed by data packets */
#define MOUSE_STREAM_MODE      0xEA  /* Interrupt Mode */
#define MOUSE_REMOTE_MODE      0xF0  /* Polling Mode */


/* Mouse Responses */
#define MOUSE_ACK        0xFA
#define MOUSE_NACK       0xFE  /* Device detected invalid output */
#define MOUSE_ERROR      0xFC


/* Mouse Data Packets (Byte 1) */
#define LB         BIT(0)  /* Mouse Left Button */
#define RB         BIT(1)  /* Mouse Right Button */
#define MB         BIT(2)  /* Mouse Middle Button */
#define ALWAYS_1   BIT(3)  /* Is always 1 */
#define X_SIGN     BIT(4)  /* If movement is positive or negative */
#define Y_SIGN     BIT(5)  /* If movement is positive or negative */
#define X_OVFL     BIT(6)  /* If is greater than 255 or less than -256 in X */
#define Y_OVFL     BIT(7)  /* If is greater than 255 or less than -256 in Y */


/* Polling / command timing */
#define DELAY_US     20000
#define MAX_RETRIES  10

#endif /* _I8042_H_ */
