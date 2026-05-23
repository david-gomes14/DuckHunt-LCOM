#include <lcom/lcf.h>

#include "../../graphic/graphic.h"
#include "menu.h"
#include "../target.h"
#include "../game.h"

#include "../../images/menu_start.xpm"
#include "../../images/menu_quit.xpm"
#include "../../images/background.xpm"

extern game_state_t state;

extern int mouse_x;
extern int mouse_y;

static int menu_selection;

static void enter_playing() {

    draw_pixmap((xpm_map_t) background_xpm, 0, 0);

    vg_store_background();

    mouse_x = 400;
    mouse_y = 300;

    target_init();

    state = STATE_PLAYING;
}

void menu_init() {
    menu_selection = 0;
}

void menu_draw() {

    if (menu_selection == 0)
        draw_pixmap((xpm_map_t) menu_start, 0, 0);

    else
        draw_pixmap((xpm_map_t) menu_quit, 0, 0);

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
        enter_playing();
        return true;
    }
    return false;
}
