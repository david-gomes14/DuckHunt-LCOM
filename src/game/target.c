#include <lcom/lcf.h>
#include <stdlib.h>
#include <stdbool.h>

#include "target.h"
#include "../graphic/graphic.h"
#include "../timer/timer.h"
#include "../images/duck.xpm"

#define TRANSPARENT_COLOR  0xFF00FF
#define SCREEN_W           800
#define SCREEN_H           600
#define DUCK_SPEED_X       4   /* pixels/frame horizontal */
#define WOBBLE_INTERVAL    40  /* frames between vertical direction changes */

static Target targets[MAX_TARGETS];
int score = 0;

/* Cached duck pixels — loaded once, reused every frame */
static uint32_t *duck_pixels = NULL;
static xpm_image_t duck_img;


static void draw_duck(int x, int y) {
	for (int i = 0; i < duck_img.height; i++) {
		for (int j = 0; j < duck_img.width; j++) {
			uint32_t c = duck_pixels[i * duck_img.width + j];
			if ((c & 0x00FFFFFF) != TRANSPARENT_COLOR)
				draw_pixel(x + j, y + i, c);
		}
	}
}

static void spawn_one(int slot) {
	int dir = (rand() % 2 == 0) ? 1 : -1;  /* 1 = left→right, -1 = right→left */
	targets[slot].dx     = dir * DUCK_SPEED_X;
	targets[slot].dy     = (rand() % 3) - 1;  /* -1, 0 or +1 */
	targets[slot].x      = (dir == 1) ? -DUCK_W : SCREEN_W;
	targets[slot].y      = 60 + rand() % (SCREEN_H - DUCK_H - 120); /* avoid very top/bottom */
	targets[slot].wobble = 0;
	targets[slot].active = true;
}


void target_init() {
	duck_pixels = (uint32_t*) xpm_load((xpm_map_t) duck, XPM_8_8_8_8, &duck_img);
	srand(timer_counter);
	for (int i = 0; i < MAX_TARGETS; i++)
		targets[i].active = false;
	score = 0;
	spawn_one(0);
}

void target_spawn() {
	for (int i = 0; i < MAX_TARGETS; i++) {
		if (!targets[i].active) {
			spawn_one(i);
			return;
		}
	}
}

void target_update() {
	bool any_active = false;
	for (int i = 0; i < MAX_TARGETS; i++) {
		if (!targets[i].active) continue;

		targets[i].x += targets[i].dx;
		targets[i].y += targets[i].dy;

		/* vertical wobble — change dy periodically */
		targets[i].wobble++;
		if (targets[i].wobble >= WOBBLE_INTERVAL) {
			targets[i].dy   = (rand() % 3) - 1;
			targets[i].wobble = 0;
		}

		/* clamp vertical so duck stays on screen */
		if (targets[i].y < 30)              targets[i].y = 30;
		if (targets[i].y > SCREEN_H - DUCK_H - 30) targets[i].y = SCREEN_H - DUCK_H - 30;

		/* despawn when duck flies off-screen horizontally */
		if (targets[i].x > SCREEN_W || targets[i].x < -DUCK_W) {
			targets[i].active = false;
			continue;
		}

		any_active = true;
	}

	if (!any_active)
		target_spawn();
}

void target_draw() {
	for (int i = 0; i < MAX_TARGETS; i++) {
		if (targets[i].active)
			draw_duck(targets[i].x, targets[i].y);
	}
}

bool target_check_hit(int mx, int my) {
	for (int i = 0; i < MAX_TARGETS; i++) {
		if (!targets[i].active) continue;
		if (mx >= targets[i].x && mx <= targets[i].x + DUCK_W &&
		    my >= targets[i].y && my <= targets[i].y + DUCK_H) {
			targets[i].active = false;
			score++;
			target_spawn();
			return true;
		}
	}
	return false;
}
