#ifndef _LCOM_UART_H_
#define _LCOM_UART_H_

/**
 * @file uarth.h
 * @brief definitions of variables of UART 16550 (Serial Port)
 */

#define COM1_IRQ 4
#define COM2_IRQ 3

#define COM1_BASE_ADDR 0x3F8
#define COM2_BASE_ADDR 0x2F8

#define RDA_MASK 0x0E

#define RATE_115200 115200



/*
* UART Base address offsets (from reading from 0x3F8 or 0x2F8)
*/
#define RECEIVER_BUFFER_REGISTER 0
#define TRANSMITTER_HOLDING_REGISTER 0
#define INTERRUPT_ENABLE_REGISTER 1
#define INTERRUPT_IDENTIFICATION_REGISTER 2
#define FIFO_CONTROL_REGISTER 2
#define LINE_CONTROL_REGISTER 3
#define MODEM_CONTROL_REGISTER 4
#define LINE_STATUS_REGISTER 5
#define MODEM_STATUS_REGISTER 6
#define SCRATCH_REGISTER 7

/*
* UART Line Control Register bits
*/
#define NUMBEROF_BITS_PER_CHAR BIT(0) | BIT(1)
#define NUMBEROF_STOP_BITS BIT(2)
#define PARITY_ENABLE BIT(3) | BIT(4) | BIT(5)
#define BREAK_CONTROL BIT(6)
#define DLAB BIT(7)


/*
* UART line Status Register bits
*/
#define DATA_READY BIT(0)
#define OVERRUN_ERROR BIT(1)
#define PARITY_ERROR BIT(2)
#define FRAMING_ERROR BIT(3)
#define BREAK_INTERRUPT BIT(4)
#define THR_EMPTY BIT(5)
#define TRANSMITTER_EMPTY BIT(6)
#define FIFO_ERROR BIT(7)


/*
* UART interrupt identification register bits
*/
#define INTERRUPT_PENDING BIT(0)
#define REIVED_DATA_AVAILABLE_MASK BIT(2) 
#define MODEM_STATUS_INTERRUPT BIT(3)


#define GAME_OVER 0xFA
#define GAME_START 0xFB
#endif /* _LCOM_UART_H_ */
