#ifndef KBC_H
#define KBC_H

#include <lcom/lcf.h>

#define ESC_BREAK_CODE 0x81

extern uint8_t scancode;
extern bool scancode_error;

int (kbc_subscribe_int)(uint8_t *bit_no);
int (kbc_unsubscribe_int)();

int (read_kbc_command)(uint8_t *value);
int (write_kbc_command)(uint8_t cmd);

#endif
