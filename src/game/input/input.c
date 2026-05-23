#include <lcom/lcf.h>

#include "input.h"

#include "../../keyboard/kbc.h"
#include "../../mouse/mouse.h"
#include "../menu/menu.h"
#include "../state/state.h"
 #include "../target.h"

extern game_state_t state;

extern int mouse_x;
extern int mouse_y;

static uint8_t mouse_packet[3];
static uint8_t mouse_index = 0;
struct packet pp;


bool handle_keyboard() {
    kbc_ih();
	if (scancode_error) {
        return true;
    }
	
    if (state == STATE_MENU) {
		switch (scancode) {
			case W_MAKE:  /* W make */
			case UP_ARROW_MAKE:  /* UP arrow second byte */
				if (menu_move_up()) menu_draw();
				break;
			case S_MAKE:  /* S make */
			case DOWN_ARROW_MAKE:  /* DOWN arrow second byte */
				if (menu_move_down()) menu_draw();
				break;
			case ENTER_MAKE:  /* ENTER make */
				if (!menu_select()) return false;
				break;
			case ESC_BREAK_CODE:
				return false;
				break;
		}
	} else {
		if (scancode == ESC_BREAK_CODE) return false;
	}

    return true;
}

void handle_mouse() {
    mouse_ih();

    if (mouse_error || state != STATE_PLAYING) {
        return;
    }

    if (!mouse_sync_packet(mouse_packet, &mouse_index)) {
        return;
    }

	mouse_parse_packet(mouse_packet, &pp);
	if (!pp.x_ov && !pp.y_ov) {
		mouse_x += pp.delta_x;
		mouse_y -= pp.delta_y;
		if (mouse_x < 0)    mouse_x = 0;
		if (mouse_y < 0)    mouse_y = 0;
		if (mouse_x >= 800) mouse_x = 799;
		if (mouse_y >= 600) mouse_y = 599;
	}
	if (pp.lb) {
		target_check_hit(mouse_x, mouse_y);
    }
}
