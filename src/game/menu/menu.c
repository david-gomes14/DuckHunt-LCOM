#include <lcom/lcf.h>

#include "menu.h"

#include "../../graphic/graphic.h"
#include "../state/state.h"

#include "../../images/menu_start.xpm"
#include "../../images/menu_quit.xpm"

static int menu_selection;

void menu_init() {
    menu_selection = 0;
}

void menu_draw() {

    if (menu_selection == 0)
        draw_pixmap((xpm_map_t) menu_start_xpm, 0, 0);

    else
        draw_pixmap((xpm_map_t) menu_quit_xpm, 0, 0);

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
        state_enter_playing();
        return true;
    }
    return false;
}
