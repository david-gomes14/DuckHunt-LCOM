#include <lcom/lcf.h>
#include <stdlib.h>
#include <stdbool.h>

#include "target.h"
#include "../../graphic/graphic.h"
#include "../../timer/timer.h"
#include "../score/score.h"

#include "../../images/duck_up.xpm"
#include "../../images/duck_down.xpm"
#include "../../images/gold_duck_up.xpm"
#include "../../images/gold_duck_down.xpm"

#define SCREEN_W 1152
#define SCREEN_H 864
#define DUCK_SPEED_X 4       // pixels por frame na horizontal
#define WOBBLE_INTERVAL 40   // frames entre mudanças de direção vertical
#define ANIM_INTERVAL 15     // frames entre troca de sprite (asa cima/baixo)
#define GOLD_CHANCE 20       // chance do pato ser dourado 1 em x

static Target targets[MAX_TARGETS];

// sprites carregados uma vez no target_init e reutilizados em cada frame
static uint32_t *duck_up_pixels = NULL;
static uint32_t *duck_down_pixels = NULL;
static uint32_t *gold_duck_up_pixels = NULL;
static uint32_t *gold_duck_down_pixels = NULL;

static xpm_image_t duck_up_info;
static xpm_image_t duck_down_info;
static xpm_image_t gold_duck_up_info;
static xpm_image_t gold_duck_down_info;


static void draw_duck(int x, int y, bool flap_up, bool facing_left, bool is_gold) {
	uint32_t *pixels;
	xpm_image_t *info;
	if (is_gold) {
		pixels = flap_up ? gold_duck_up_pixels : gold_duck_down_pixels;
		info = flap_up ? &gold_duck_up_info  : &gold_duck_down_info;
	} else {
		pixels = flap_up ? duck_up_pixels : duck_down_pixels;
		info = flap_up ? &duck_up_info : &duck_down_info;
	}
	draw_sprite(pixels, info, x, y, facing_left);
}

static void spawn_one(int slot) {
	int dir = (rand() % 2 == 0) ? 1 : -1;  // 1 = esquerda→direita, -1 = direita→esquerda
	targets[slot].anim_frame = 0;
	targets[slot].dx = dir * DUCK_SPEED_X;
	targets[slot].dy = (rand() % 3) - 1;   // velocidade vertical: -1, 0 ou +1
	targets[slot].x = (dir == 1) ? -DUCK_W : SCREEN_W;  // entra fora do ecrã
	targets[slot].y = 220 + rand() % (SCREEN_H - DUCK_H - 280);  // abaixo das placas do HUD
	targets[slot].wobble = 0;
	targets[slot].active = true;
	targets[slot].is_gold = (rand() % GOLD_CHANCE == 0);
}


void target_init() {
	duck_up_pixels = (uint32_t*) xpm_load((xpm_map_t) duck_up_xpm, XPM_8_8_8_8, &duck_up_info);
	duck_down_pixels = (uint32_t*) xpm_load((xpm_map_t) duck_down_xpm, XPM_8_8_8_8, &duck_down_info);

	gold_duck_up_pixels = (uint32_t*) xpm_load((xpm_map_t) gold_duck_up_xpm, XPM_8_8_8_8, &gold_duck_up_info);
	gold_duck_down_pixels = (uint32_t*) xpm_load((xpm_map_t) gold_duck_down_xpm, XPM_8_8_8_8, &gold_duck_down_info);

	srand(timer_counter);  // seed baseada no tempo para direções diferentes a cada jogo
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

		// muda a direção vertical de vez em quando para o pato não voar em linha reta
		targets[i].wobble++;
		if (targets[i].wobble >= WOBBLE_INTERVAL) {
			targets[i].dy = (rand() % 3) - 1;
			targets[i].wobble = 0;
		}

		// mantém o pato abaixo das placas do HUD e acima do fundo
		if (targets[i].y < 220) targets[i].y = 220;
		if (targets[i].y > SCREEN_H - DUCK_H - 30) targets[i].y = SCREEN_H - DUCK_H - 30;

		// quando sai pelo lado, desativa para poder ser substituído
		if (targets[i].x > SCREEN_W || targets[i].x < -DUCK_W) {
			targets[i].active = false;
			continue;
		}
	}

	// garante que há sempre MAX_TARGETS patos ativos em simultâneo
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
		draw_duck(targets[i].x, targets[i].y, flap_up, facing_left, targets[i].is_gold);
	}
}

bool target_check_hit(int mx, int my) {
	for (int i = 0; i < MAX_TARGETS; i++) {
		if (!targets[i].active) continue;
		if (mx >= targets[i].x && mx <= targets[i].x + DUCK_W &&
		    my >= targets[i].y && my <= targets[i].y + DUCK_H) {
			targets[i].active = false;
			score_add(targets[i].is_gold ? 5 : 1);
			spawn_one(i);
			return true;
		}
	}
	return false;
}
