#include <lcom/lcf.h>

#include "input.h"

#include "../../keyboard/kbc.h"
#include "../../mouse/mouse.h"
#include "../menu/menu.h"
#include "../state/state.h"
 #include "../target/target.h"

extern game_state_t state;

extern int mouse_x;
extern int mouse_y;

static uint8_t mouse_packet[3];
static uint8_t mouse_index = 0;
struct packet pp;


void handle_keyboard() {
    kbc_ih();
	if (scancode_error) return;

    if (state == STATE_MENU) {
		switch (scancode) {
			case W_MAKE:
			case UP_ARROW_MAKE:  // setas de 2 bytes: 0xE0 é ignorado, este é o 2º byte
				if (menu_move_up()) menu_draw();
				break;
			case S_MAKE:
			case DOWN_ARROW_MAKE:
				if (menu_move_down()) menu_draw();
				break;
			case ENTER_MAKE:
				if (!menu_select()) state = STATE_EXIT;  // menu_select devolve false no "Quit"
				break;
			case ESC_BREAK_CODE:
				state = STATE_EXIT;
				break;
		}
	} else if (state == STATE_GAME_OVER) {
		if (scancode == ENTER_MAKE)
			state_enter_leaderboard();
		else if (scancode == ESC_BREAK_CODE)
			state_enter_menu();
	} else if (state == STATE_LEADERBOARD) {
		if (scancode == ENTER_MAKE || scancode == ESC_BREAK_CODE)
			state_enter_menu();
	} else {
		if (scancode == ESC_BREAK_CODE) state = STATE_EXIT;
	}
}

void handle_mouse() {
	static bool last_lb = false;

    mouse_ih();

    if (mouse_error || state != STATE_PLAYING) return;

    // acumula os 3 bytes do pacote PS/2; só processa quando o pacote está completo
    if (!mouse_sync_packet(mouse_packet, &mouse_index)) return;

	mouse_parse_packet(mouse_packet, &pp);

	// ignora pacotes com overflow (delta demasiado grande, causaria salto)
	if (!pp.x_ov && !pp.y_ov) {
		mouse_x += pp.delta_x * 3 / 4;  // fator 3/4 para rato físico não ficar demasiado rápido
		mouse_y -= pp.delta_y * 3 / 4;  // y invertido: rato sobe = cursor sobe no ecrã
		if (mouse_x < 0) mouse_x = 0;
		if (mouse_y < 0) mouse_y = 0;
		if (mouse_x >= 1152) mouse_x = 1151;
		if (mouse_y >= 864) mouse_y = 863;
	}

	// edge detection: só dispara no momento em que o botão é premido, não enquanto está segurado
	if (pp.lb && !last_lb)
		target_check_hit(mouse_x, mouse_y);

	last_lb = pp.lb;
}
