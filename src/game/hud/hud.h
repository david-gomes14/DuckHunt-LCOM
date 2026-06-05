#ifndef HUD_H
#define HUD_H

#include <stdint.h>

void hud_init();
void hud_draw(int score, int time_left);
void hud_draw_score_at(int score, int x, int y);
void hud_draw_score_large_at(int score, int x, int y);
void hud_draw_number_at(int n, int x, int y);
void hud_draw_datetime_at(uint8_t hour, uint8_t minute, uint8_t day, uint8_t month, int x, int y);

#endif
