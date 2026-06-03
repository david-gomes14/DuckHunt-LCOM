#include <lcom/lcf.h>

#include "uart.h"
#include "serial_port.h"
#include "../utils/utils.h"

int serial_port_hook_id = COM1_IRQ;
uint8_t serial_port_byte;
bool serial_port_error;

int (serial_port_subscribe_int)(uint8_t * bit_no) {
    if(bit_no == NULL) return 1;

    *bit_no = serial_port_hook_id;
    if(sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &serial_port_hook_id) != 0) return 1;

    return 0;
}

int (serial_port_unsubscribe_int)() {
    if(sys_irqrmpolicy(&serial_port_hook_id) != 0) return 1;
    return 0;
}

int (serial_port_set_baud_rate)(uint16_t baud_rate) {
    uint8_t lcr;
    if(util_sys_inb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, &lcr) != 0) return 1;

    lcr |= DLAB; //activate DLAB to set the baud rate



    
    return 0;
}

int (serial_port_write_cmd)(uint8_t cmd) {}

int (serial_port_read_data)(uint8_t *data) {}

void (serial_port_ih)();



