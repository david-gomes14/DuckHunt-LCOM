#include <lcom/lcf.h>

#include "state.h"

#include "../../graphic/graphic.h"
#include "../../serial_port/uart.h"
#include "../menu/menu.h"
#include "../target/target.h"
#include "../hud/hud.h"
#include "../score/score.h"

#include "../../serial_port/serial_port.h"
#include "../../images/background.xpm"
#include "../../images/game_over.xpm"

#define CROSSHAIR_COLOR 0xFF0000
#define GAME_DURATION_TICKS (60 * 60)
#define GAMEOVER_SCORE_X 618
#define GAMEOVER_SCORE_Y 473

#define SCREEN_W 1152
#define SCREEN_H 864

game_state_t state = STATE_MENU;
game_mode_t mode = MODE_SINGLEPLAYER;

extern int mouse_x;
extern int mouse_y;

static int ticks_remaining = 0;

void state_render() {
    vg_restore_background();
	if (state == STATE_PLAYING) {
        hud_draw(score_get(), state_get_time());
		target_update();
		target_draw();
		draw_crosshair(mouse_x, mouse_y, 10, CROSSHAIR_COLOR);
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
    draw_pixmap((xpm_map_t) background, 0, 0);
    vg_store_background();
    hud_init();
    hud_draw(score_get(), state_get_time());
    mouse_x = SCREEN_W / 2;
    mouse_y = SCREEN_H / 2;
    target_init();
    score_reset();
    ticks_remaining = GAME_DURATION_TICKS;

    if(mode == MODE_MULTIPLAYER_HOST) {
        serial_port_write_byte(GAME_START);
    }

    state = STATE_PLAYING;
}

void state_enter_game_over() {
    draw_pixmap((xpm_map_t) game_over, 0, 0);
    hud_draw_score_large_at(score_get(), GAMEOVER_SCORE_X, GAMEOVER_SCORE_Y);
    vg_store_background();
    state = STATE_GAME_OVER;
}

void state_tick() {
    if (state != STATE_PLAYING) return;
    ticks_remaining--;
    if (ticks_remaining <= 0) {
        state_enter_game_over();
    }
    if (score_get() == 10) {
        if (mode == MODE_MULTIPLAYER_HOST || mode == MODE_MULTIPLAYER_GUEST) {
            serial_port_write_byte(GAME_OVER);
        }

        state_enter_game_over();
        state = STATE_GAME_OVER;
    }
}

int state_get_time() {
    return ticks_remaining / 60;
}

