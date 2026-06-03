#ifndef HUD_H
#define HUD_H

void hud_init();
void hud_draw(int score, int time_left);
void hud_draw_score_at(int score, int x, int y);
void hud_draw_score_large_at(int score, int x, int y);

#endif
