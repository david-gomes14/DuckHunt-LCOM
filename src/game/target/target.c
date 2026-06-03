#include <lcom/lcf.h>
#include <stdlib.h>
#include <stdbool.h>

#include "target.h"
#include "../../graphic/graphic.h"
#include "../../timer/timer.h"
#include "../score/score.h"

#include "../../images/duck_up.xpm"
#include "../../images/duck_down.xpm"

#define SCREEN_W           1152
#define SCREEN_H           864
#define DUCK_SPEED_X       4   /* pixels/frame horizontal */
#define WOBBLE_INTERVAL    40  /* frames between vertical direction changes */
#define ANIM_INTERVAL      15  /* frames entre mudança de sprite */

static Target targets[MAX_TARGETS];

/* Cached duck pixels — loaded once, reused every frame */
static uint32_t *duck_up_pixels = NULL;
static uint32_t *duck_down_pixels = NULL;
static xpm_image_t duck_up_info;
static xpm_image_t duck_down_info;


static void draw_duck(int x, int y, bool flap_up, bool facing_left) {
	uint32_t *pixels = flap_up ? duck_up_pixels : duck_down_pixels;
	xpm_image_t *info = flap_up ? &duck_up_info : &duck_down_info;
	draw_pixels(pixels, info, x, y, facing_left);
}

static void spawn_one(int slot) {
	int dir = (rand() % 2 == 0) ? 1 : -1;  /* 1 = left→right, -1 = right→left */
	targets[slot].anim_frame = 0;
	targets[slot].dx     = dir * DUCK_SPEED_X;
	targets[slot].dy     = (rand() % 3) - 1;  /* -1, 0 or +1 */
	targets[slot].x      = (dir == 1) ? -DUCK_W : SCREEN_W;
	targets[slot].y      = 60 + rand() % (SCREEN_H - DUCK_H - 120); /* avoid very top/bottom */
	targets[slot].wobble = 0;
	targets[slot].active = true;
}


void target_init() {
	duck_up_pixels = (uint32_t*) xpm_load((xpm_map_t) duck_up_xpm, XPM_8_8_8_8, &duck_up_info);
	duck_down_pixels = (uint32_t*) xpm_load((xpm_map_t) duck_down_xpm, XPM_8_8_8_8, &duck_down_info);
	srand(timer_counter);
	for (int i = 0; i < MAX_TARGETS; i++) {
		targets[i].active = false;
		spawn_one(i);
	}
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
	for (int i = 0; i < MAX_TARGETS; i++) {
		if (!targets[i].active) continue;

		targets[i].anim_frame++;
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
	}

	int active_count = 0;
	for (int i = 0; i < MAX_TARGETS; i++) {
		if (targets[i].active)
			active_count++;
	}

	while (active_count < MAX_TARGETS) {
		target_spawn();
		active_count++;
	}
}

void target_draw() {
  	for (int i = 0; i < MAX_TARGETS; i++) {
		if (!targets[i].active) continue;
		bool flap_up = (targets[i].anim_frame / ANIM_INTERVAL) % 2 == 0;
		bool facing_left = targets[i].dx < 0;
		draw_duck(targets[i].x, targets[i].y, flap_up, facing_left);
	}
}

bool target_check_hit(int mx, int my) {
	for (int i = 0; i < MAX_TARGETS; i++) {
		if (!targets[i].active) continue;
		if (mx >= targets[i].x && mx <= targets[i].x + DUCK_W &&
		    my >= targets[i].y && my <= targets[i].y + DUCK_H) {
			targets[i].active = false;
			score_add(1);
			spawn_one(i);
			return true;
		}
	}
	return false;
}
