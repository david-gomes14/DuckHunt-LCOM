#include <lcom/lcf.h>
#include <minix/sysutil.h>

#include "i8042.h"
#include "kbc.h"
#include "../utils/utils.h"

int kbc_hook_id = KBC_IRQ;
uint8_t scancode;
bool scancode_error;

int (kbc_subscribe_int)(uint8_t *bit_no) {
	if (bit_no == NULL) return 1;

	*bit_no = kbc_hook_id;
	if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id) != 0) return 1;

	return 0;
}

int (kbc_unsubscribe_int)() {
	if (sys_irqrmpolicy(&kbc_hook_id) != 0) return 1;
	return 0;
}

int (read_kbc_command)(uint8_t *value) {

    uint8_t status;
    int retries = MAX_RETRIES;
    
    while (retries) {

        if (util_sys_inb(KBC_STATUS_REG, &status) != 0) {
		    return 1;
	    }

        if (status & (TIMEOUT | PARITY)) {
		    return 1;
	    }

        if ((status & OUTPUT_BUFFER_FULL) != 0) {

            if (util_sys_inb(OUTPUT_BUF_REG, value) != 0) {
                return 1;
            }
            
            if (status & BIT(5)) { 
                return 1;
            }
            return 0;

        }

        retries--;
    }

    return 1;
}


int (write_kbc_command)(uint8_t cmd) {

    uint8_t status;
    int retries = MAX_RETRIES;
    
    while (retries) {

        if (util_sys_inb(KBC_STATUS_REG, &status) != 0) {
		    return 1;
	    }

        if (status & (TIMEOUT | PARITY)) {
		    return 1;
	    }

        if ((status & INPUT_BUFFER_FULL) == 0) {

            if (sys_outb(KBC_COMMAND_REG, cmd) != 0) {
                return 1;
            }
            
            if (status & BIT(5)) { 
                return 1;
            }
            return 0;

        }

        retries--;
    }

    return 1;
}


void (kbc_ih)() {

    if (read_kbc_command(&scancode) != 0) {
        scancode_error = true;
        return;
    }

    scancode_error = false;
}

