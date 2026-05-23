#include <lcom/lcf.h>

#include "../../keyboard/kbc.h"
#include "../../mouse/mouse.h"
#include "../menu/menu.h"
#include "../target.h"
#include "../game.h"
#include "input.h"

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
			case 0x11:  /* W make */
			case 0x48:  /* UP arrow second byte */
				if (menu_move_up()) menu_draw();
				break;
			case 0x1F:  /* S make */
			case 0x50:  /* DOWN arrow second byte */
				if (menu_move_down()) menu_draw();
				break;
			case 0x1C:  /* ENTER make */
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
