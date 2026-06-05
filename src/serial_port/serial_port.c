#include <lcom/lcf.h>

#include "uart.h"
#include "serial_port.h"
#include "../utils/utils.h"
#include "../game/state/state.h"

extern game_state_t state;

int serial_port_hook_id = COM1_IRQ;
uint8_t serial_port_data_available;

int (serial_port_connect)(uint32_t baud_rate, uint8_t *bit_no) {
    if(serial_port_set_baud_rate(baud_rate) != 0) return 1;
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

int (serial_port_set_baud_rate)(uint32_t baud_rate) {
    uint8_t lcr;
    if(util_sys_inb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, &lcr) != 0) return 1;

    lcr |= DLAB; 

    if(sys_outb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, lcr) != 0) return 1;

    uint16_t divisor = RATE_115200 / baud_rate;
    uint8_t dll = divisor & 0xFF;
    uint8_t dlm = (divisor & 0xFF00) >> 8;

    if(sys_outb(COM1_BASE_ADDR, dll) != 0) return 1;
    if(sys_outb(COM1_BASE_ADDR + 1, dlm) != 0) return 1;
 
    lcr &= ~DLAB;

    if(sys_outb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, lcr) != 0) return 1;
    
    return 0;
}

int (serial_port_format_data)(){
    uint8_t lcr;
    if(util_sys_inb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, &lcr) != 0) return 1;

    lcr |= BIT(0) | BIT(1);
    uint8_t mask = BIT(2) | BIT(3) | BIT(4) | BIT(5);
    lcr &= ~mask;

    if(sys_outb(COM1_BASE_ADDR + LINE_CONTROL_REGISTER, lcr) != 0) return 1;

    return 0;
}

int (serial_port_enableinterrupts)() {
    uint8_t ier;
    
    if(util_sys_inb(COM1_BASE_ADDR + INTERRUPT_ENABLE_REGISTER, &ier) != 0) return 1;

    ier |= BIT(0);

    if(sys_outb(COM1_BASE_ADDR + INTERRUPT_ENABLE_REGISTER, ier) != 0) return 1;

    return 0;
}

int (serial_port_read_byte)(uint8_t *byte) {
    uint8_t lsr;
    if(util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REGISTER, &lsr) != 0) return 1;

    if((lsr & (OVERRUN_ERROR | PARITY_ERROR | FRAMING_ERROR)) != 0) return 1;

    if(util_sys_inb(COM1_BASE_ADDR, byte) != 0) return 1;

    return 0;
}

int (serial_port_write_byte)(uint8_t byte) {
    uint8_t lsr;

    do {
        if(util_sys_inb(COM1_BASE_ADDR + LINE_STATUS_REGISTER, &lsr) != 0) return 1;

        if((lsr & (THR_EMPTY)) == 0) {
            tickdelay(micros_to_ticks(1000));
        }

    }while((lsr & THR_EMPTY) == 0);


    if(sys_outb(COM1_BASE_ADDR, byte) != 0) return 1;

    return 0;
}

void (handle_uart)() {
    serial_port_ih();

    if(serial_port_data_available == GAME_OVER) {
        serial_port_data_available = 0; 

        state_enter_game_over();

        state = STATE_GAME_OVER;
    }

    if(serial_port_data_available == GAME_START) {
        serial_port_data_available = 0;

        state_enter_playing();

        state = STATE_PLAYING;
    }
}

void (serial_port_ih)() {
    uint8_t iir;

    if(util_sys_inb(COM1_BASE_ADDR + INTERRUPT_IDENTIFICATION_REGISTER, &iir) != 0) return;

    if ((iir & RDA_MASK) != 0x04 && (iir & RDA_MASK) != 0x0C) return;

    serial_port_read_byte(&serial_port_data_available);

    return;
}
