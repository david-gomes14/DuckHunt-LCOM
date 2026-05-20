#include <lcom/lcf.h>

#include "i8042.h"
#include "mouse.h"
#include "../utils/utils.h"

int mouse_hook_id = MOUSE_IRQ;
uint8_t mouse_byte;
bool mouse_error;

int mouse_x = 400;
int mouse_y = 300;


int (mouse_subscribe_int)(uint8_t *bit_no) {
	if (bit_no == NULL) return 1;

	*bit_no = mouse_hook_id;
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != 0) return 1;

	return 0;
}


int (mouse_unsubscribe_int)() {
	if (sys_irqrmpolicy(&mouse_hook_id) != 0) return 1;
	return 0;
}


int (mouse_write_cmd)(uint8_t cmd) {
	uint8_t response;
	int retries = MAX_RETRIES;
	uint8_t status;

	while(retries--) {
		if (util_sys_inb(KBC_STATUS_REG, &status) != 0) return 1;

		if ((status & INPUT_BUFFER_FULL) == 0) {
			if (sys_outb(KBC_COMMAND_REG, WRITE_TO_MOUSE) != 0) return 1;
		}

		else {
			tickdelay(micros_to_ticks(DELAY_US));
			continue;
		}

		if(util_sys_inb(KBC_STATUS_REG, &status) != 0) return 1;

		if((status & INPUT_BUFFER_FULL) == 0) {
			if(sys_outb(INPUT_BUF_REG, cmd) != 0) return 1;
		}

		else {
			tickdelay(micros_to_ticks(DELAY_US));
			continue;
		}

		if(util_sys_inb(KBC_STATUS_REG, &status) != 0) return 1;

		if(status & OUTPUT_BUFFER_FULL) {
			if(util_sys_inb(OUTPUT_BUF_REG, &response) != 0) return 1;
		}

		else {
			tickdelay(micros_to_ticks(DELAY_US));
			continue;
		}

		if (status & (PARITY | TIMEOUT)) continue;

		if (response == MOUSE_ACK) return 0;

		if (response == MOUSE_NACK || response == MOUSE_ERROR) continue;
	}

	return 1;
}


int (mouse_enable_data)() {
	return mouse_write_cmd(MOUSE_ENABLE_DATA);
}


int (mouse_disable_data)() {
	return mouse_write_cmd(MOUSE_DISABLE_DATA);
}


bool (mouse_sync_packet)(uint8_t packet_bytes[3], uint8_t *packet_index){
	if (packet_bytes == NULL || packet_index == NULL) return false;

	if (*packet_index == 0 && !(mouse_byte & ALWAYS_1)) {
		return false;    
	}

	packet_bytes[*packet_index] = mouse_byte;
	(*packet_index)++;

	if (*packet_index == 3) {
		*packet_index = 0;
		return true;
	}

	return false;
}



void (mouse_parse_packet)(uint8_t packet_bytes[3], struct packet *pp) {

	if (packet_bytes == NULL || pp == NULL) return;

	pp->bytes[0] = packet_bytes[0];
	pp->bytes[1] = packet_bytes[1];
	pp->bytes[2] = packet_bytes[2];

	pp->lb = packet_bytes[0] & LB;
	pp->rb = packet_bytes[0] & RB;
	pp->mb = packet_bytes[0] & MB;

	pp->x_ov = packet_bytes[0] & X_OVFL;
	pp->y_ov = packet_bytes[0] & Y_OVFL;

	if (packet_bytes[0] & X_SIGN) {
		pp->delta_x = (int16_t)(0xFF00 | packet_bytes[1]);
	} else {
		pp->delta_x = packet_bytes[1];
	}

	if (packet_bytes[0] & Y_SIGN) {
		pp->delta_y = (int16_t)(0xFF00 | packet_bytes[2]);
	} else {
		pp->delta_y = packet_bytes[2];
	}
}


void (mouse_ih)(){
	uint8_t status;
	mouse_error = false;

	if (util_sys_inb(KBC_STATUS_REG, &status) != 0) {
		mouse_error = true;
		return;
	}

	if (!(status & OUTPUT_BUFFER_FULL)) {
		mouse_error = true;
		return;
	}

	if (!(status & AUX)) {
		mouse_error = true;
		return;
	}

	if (status & (PARITY | TIMEOUT)) {
		mouse_error = true;
	}

	if (util_sys_inb(OUTPUT_BUF_REG, &mouse_byte) != 0) {
		mouse_error = true;
		return;
	}

}



