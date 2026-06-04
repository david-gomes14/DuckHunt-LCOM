#include <lcom/lcf.h>

#include "uart.h"
#include "serial_port.h"
#include "../utils/utils.h"

int serial_port_hook_id = COM1_IRQ;
uint8_t serial_port_byte;
bool serial_port_error;

int (serial_port_connect)(uint16_t baud_rate, uint8_t *bit_no) {
    if(serialport_set_baud_rate(baud_rate) != 0) return 1;
    if(serial_port_format_data() != 0) return 1;
    if(serial_port_enableinterrupts() != 0) return 1;
    if(serial_port_subscribe_int(bit_no) != 0) return 1;

    return 0;
}

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

    lcr |= DLAB; 

    if(sys_outb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, &lcr) != 0) return 1;

    uint8_t dll = baud_rate & 0xFF;
    uint8_t dlm = (baud_rate & 0xFF00) >> 8;

    if(sys_outb(COM1_BASE_ADDR, &dll) != 0) return 1;
    if(sys_outb(COM1_BASE_ADDR + 1, &dlm) != 0) return 1;
 
    lcr &= ~DLAB;

    if(sys_outb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, &lcr) != 0) return 1;
    
    return 0;
}

int (serial_port_format_data)(){
    uint8_t lcr;
    if(util_sys_inb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, &lcr) != 0) return 1;

    lcr |= BIT(0) | BIT(1);
    uint8_t mask = BIT(2) | BIT(3) | BIT(4) | BIT(5);
    lcr &= ~mask;

    if(sys_outb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, &lcr) != 0) return 1;

    return 0;
}

int (serial_port_enableinterrupts)() {
    uint8_t ier;
    
    if(util_sys_inb(COM1_BASE_ADDR + INTERRUPT_ENABLE_REGISTER, &ier) != 0) return 1;

    ier |= BIT(0);

    if(sys_outb(COM1_BASE_ADDR + INTERRUPT_ENABLE_REGISTER, &ier) != 0) return 1;

    return 0;
}

int (serial_port_read_byte)(uint8_t *byte) {
    uint8_t lsr;
    if(util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REGISTER, &lsr) != 0) return 1;

    if((lsr & (OVERRUN_ERROR | PARITY_ERROR | FRAMING_ERROR)) != 0) return 1;
    if((lsr & DATA_READY) == 0) return 1;

    if(util_sys_inb(COM1_BASE_ADDR, byte) != 0) return 1;

    return 0;
}

void (serial_port_ih)();



