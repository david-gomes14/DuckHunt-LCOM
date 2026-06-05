#ifndef TARGET_H
#define TARGET_H

#include <stdbool.h>

#define MAX_TARGETS 5
#define DUCK_W 86
#define DUCK_H 86

typedef struct {
	int x, y;
	int dx, dy;
	int wobble;
	int anim_frame;
	bool active;
	bool is_gold;
} Target;

void target_init();
void target_spawn();
void target_update();
void target_draw();
bool target_check_hit(int mx, int my);

#endif
