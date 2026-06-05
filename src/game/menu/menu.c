#include <lcom/lcf.h>

#include "menu.h"

#include "../../graphic/graphic.h"
#include "../state/state.h"

#include "../../images/menu_start.xpm"
#include "../../images/menu_quit.xpm"

static int menu_selection;

extern game_mode_t mode;

static uint32_t *menu_start_pixels = NULL;
static uint32_t *menu_quit_pixels  = NULL;
static xpm_image_t menu_start_info;
static xpm_image_t menu_quit_info;

void menu_init() {
    menu_selection = 0;
    if (menu_start_pixels == NULL)
        menu_start_pixels = (uint32_t*) xpm_load((xpm_map_t) menu_start_xpm, XPM_8_8_8_8, &menu_start_info);
    if (menu_quit_pixels == NULL)
        menu_quit_pixels  = (uint32_t*) xpm_load((xpm_map_t) menu_quit_xpm,  XPM_8_8_8_8, &menu_quit_info);
}

void menu_draw() {
    uint32_t *pixels = (menu_selection == 0) ? menu_start_pixels : menu_quit_pixels;
    xpm_image_t *info = (menu_selection == 0) ? &menu_start_info : &menu_quit_info;

    draw_sprite(pixels, info, 0, 0, false);

    vg_store_background();
}

bool menu_move_up() {
    if (menu_selection == 0) return false;
    menu_selection = 0;
    return true;
}
bool menu_move_down() {
    if (menu_selection == 1) return false;
    menu_selection = 1;
    return true;
}

bool menu_select() {
    if (menu_selection == 0) {
        mode = MODE_SINGLEPLAYER;
        state_enter_playing();
        return true;
    }
    if(menu_selection == 1) {
        mode = MODE_MULTIPLAYER_HOST;
        state_enter_playing();
        return true;
    }
    return false;
}
