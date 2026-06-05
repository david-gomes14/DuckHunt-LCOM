#ifndef STATE_H
#define STATE_H

typedef enum {
    STATE_MENU = 0,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_LEADERBOARD,
    STATE_EXIT
} game_state_t;

extern game_state_t state;

void state_render();
void state_enter_menu();
void state_enter_playing();
void state_enter_game_over();
void state_enter_leaderboard();
void state_tick();
int state_get_time();

#endif
