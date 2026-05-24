#ifndef TARGET_H
#define TARGET_H

#include <stdbool.h>

#define MAX_TARGETS  2
#define DUCK_W       60
#define DUCK_H       60

typedef struct {
	int x, y;
	int dx, dy;
	int wobble;
	int anim_frame;
	bool active;
} Target;

extern int score;

void target_init();
void target_spawn();
void target_update();
void target_draw();
bool target_check_hit(int mx, int my);

#endif
