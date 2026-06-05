#ifndef STATE_H
#define STATE_H

typedef enum {
    STATE_MENU = 0,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_EXIT
} game_state_t;

typedef enum {
    MODE_SINGLEPLAYER,
    MODE_MULTIPLAYER_HOST,
    MODE_MULTIPLAYER_GUEST
} game_mode_t;

extern game_state_t state;
extern game_mode_t mode;

void state_render();
void state_enter_menu();
void state_enter_playing();
void state_enter_game_over();
void state_tick();
int state_get_time();

#endif
