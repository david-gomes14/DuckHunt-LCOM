#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>
#include <stdbool.h>

extern bool mouse_error;

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
int (mouse_write_cmd)(uint8_t cmd);
int (mouse_enable_data)();
int (mouse_disable_data)();
bool (mouse_sync_packet)(uint8_t packet_bytes[3], uint8_t *index);
void (mouse_parse_packet)(uint8_t packet_bytes[3], struct packet *pp);
void (mouse_ih)();

#endif
