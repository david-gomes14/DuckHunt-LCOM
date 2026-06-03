#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <lcom/lcf.h>
#include <stdbool.h>

int (serial_port_subscribe_int)(uint8_t * bit_no);
int (serial_port_unsubscribe_int)();
void (serial_port_ih)();

int (serial_port_set_baud_rate)(uint16_t baud_rate);
int (serial_port_format_data)();

int (serial_port_write_cmd)(uint8_t cmd);

int (serial_port_read_data)(uint8_t *data);

#endif
