#include <lcom/lcf.h>

#include "menu.h"

#include "../../graphic/graphic.h"
#include "../state/state.h"

#include "../../images/menu_singleplayer.xpm"
#include "../../images/menu_multiplayer.xpm"
#include "../../images/menu_leaderboard.xpm"
#include "../../images/menu_quit.xpm"

static int menu_selection;

extern game_mode_t mode;

static uint32_t *menu_pixels[4];
static xpm_image_t menu_info[4];

void menu_init() {
    menu_selection = 0;
    if (menu_pixels[0] == NULL) {
        menu_pixels[0] = (uint32_t*) xpm_load((xpm_map_t) menu_singleplayer, XPM_8_8_8_8, &menu_info[0]);
        menu_pixels[1] = (uint32_t*) xpm_load((xpm_map_t) menu_multiplayer,  XPM_8_8_8_8, &menu_info[1]);
        menu_pixels[2] = (uint32_t*) xpm_load((xpm_map_t) menu_leaderboard,  XPM_8_8_8_8, &menu_info[2]);
        menu_pixels[3] = (uint32_t*) xpm_load((xpm_map_t) menu_quit,         XPM_8_8_8_8, &menu_info[3]);
    }
}

void menu_draw() {
    draw_sprite(menu_pixels[menu_selection], &menu_info[menu_selection], 0, 0, false);
    vg_store_background();
}

bool menu_move_up() {
    if (menu_selection == 0) return false;
    menu_selection--;
    return true;
}

bool menu_move_down() {
    if (menu_selection == 3) return false;
    menu_selection++;
    return true;
}

bool menu_select() {
    if (menu_selection == 0) {
        mode = MODE_SINGLEPLAYER;
        state_enter_playing();
        return true;
    }
    if (menu_selection == 1) {
        state_enter_multiplayer();   // decide host/guest e mostra "waiting" se for host
        return true;
    }
    if (menu_selection == 2) {
        state_enter_leaderboard();
        return true;
    }
    return false;  // QUIT (índice 3) → caller trata de STATE_EXIT
}
