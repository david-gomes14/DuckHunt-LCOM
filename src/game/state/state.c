#include <lcom/lcf.h>

#include "state.h"

#include "../../graphic/graphic.h"
#include "../menu/menu.h"
#include "../target/target.h"

#include "../../images/background.xpm"

#define GAME_DURATION_TICKS (60 * 60)

game_state_t state = STATE_MENU;

extern int mouse_x;
extern int mouse_y;

static int ticks_remaining = 0;

void state_render() {
    vg_restore_background();
	if (state == STATE_PLAYING) {
		target_update();
		target_draw();
		draw_crosshair(mouse_x, mouse_y, 10, 0xFF00FF);
	}
	vg_flip();
}

void state_enter_menu() {
    ticks_remaining = 0;
    menu_init();
    menu_draw();
    state = STATE_MENU;
}

void state_enter_playing() {
    draw_pixmap((xpm_map_t) background_xpm, 0, 0);
    vg_store_background();
    mouse_x = 576;
    mouse_y = 432;
    target_init();
    ticks_remaining = GAME_DURATION_TICKS;
    state = STATE_PLAYING;
}

void state_tick() {
    if (state != STATE_PLAYING) return;
    ticks_remaining--;
    if (ticks_remaining <= 0) {
        state_enter_menu();
    }
}

int state_get_time() {
    return ticks_remaining / 60;
}

