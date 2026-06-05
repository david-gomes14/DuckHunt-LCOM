#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <lcom/lcf.h>
#include <stdbool.h>

#include "uart.h"

extern uint8_t serial_port_data_available;

int (serial_port_connect)(uint32_t baud_rate, uint8_t *bit_no);

int (serial_port_subscribe_int)(uint8_t * bit_no);
int (serial_port_unsubscribe_int)();

int (serial_port_set_baud_rate)(uint32_t baud_rate);
int (serial_port_format_data)();
int (serial_port_enableinterrupts)();

int (serial_port_read_byte)(uint8_t *byte);
int (serial_port_write_byte)(uint8_t *byte);

void (serial_port_ih)();

#endif
