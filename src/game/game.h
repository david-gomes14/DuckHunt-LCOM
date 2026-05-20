#ifndef GAME_H
#define GAME_H

typedef enum {
	STATE_MENU = 0,
	STATE_PLAYING
} game_state_t;

void game_init();
void game_loop();
void game_cleanup();

#endif

