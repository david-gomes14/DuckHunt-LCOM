#ifndef STATE_H
#define STATE_H

#include <lcom/lcf.h>

typedef enum {
    STATE_MENU = 0,
    STATE_PLAYING
} game_state_t;

extern game_state_t state;

void state_render();
void state_enter_menu();
void state_enter_playing();
void state_tick();
int state_get_time();

#endif
