#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <stdint.h>

int set_graphics_mode(uint16_t mode);

int map_video_memory(uint16_t mode);
int draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int draw_crosshair(int x, int y, int size, uint32_t color);

void draw_pixels(uint32_t *pixels, xpm_image_t *info, int x, int y, bool facing_left);
int draw_sprite(xpm_map_t xpm, int x, int y);
int draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y);

void vg_store_background();
void vg_restore_background();
void vg_flip();
void vg_free_buffer();

#endif

