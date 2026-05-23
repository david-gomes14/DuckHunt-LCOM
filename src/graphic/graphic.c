#include <lcom/lcf.h>

#include "graphic.h"

#include <stdio.h>
#include <string.h>


static char *video_mem;
static char *back_buffer;
static char *bg_buffer;
static vbe_mode_info_t vmi;
static unsigned bytes_per_pixel;
static unsigned int vram_size;

int set_graphics_mode(uint16_t mode) {

	reg86_t reg;
	memset(&reg, 0, sizeof(reg));

	reg.intno = 0x10;
	reg.ah = 0x4F;
	reg.al = 0x02;

	reg.bx = mode | BIT(14);

	if (sys_int86(&reg) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	}

	return 0;
}

int map_video_memory(uint16_t mode) {
	int r;
	struct minix_mem_range mr;

	if (vbe_get_mode_info(mode, &vmi) != 0) {
		printf("vbe_get_mode_info failed\n");
		return 1;
	}

	bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;

	unsigned int vram_base = vmi.PhysBasePtr;
	vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel;

	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;


	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
		printf("sys_privctl (ADD_MEM) failed: %d\n", r);
		return 1;
	}

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

	if(video_mem == MAP_FAILED) {
		printf("couldn't map video memory");
		return 1;
	}
   
	back_buffer = malloc(vram_size);
	if (back_buffer == NULL) {
		printf("couldn't allocate back buffer\n");
		return 1;
	}
	memset(back_buffer, 0, vram_size);

	bg_buffer = malloc(vram_size);
	if (bg_buffer == NULL) {
		printf("couldn't allocate bg buffer\n");
		return 1;
	}
	memset(bg_buffer, 0, vram_size);

	return 0;

}


int draw_pixel(uint16_t x, uint16_t y, uint32_t color) {

	if (x >= vmi.XResolution || y >= vmi.YResolution) return 1;
	
	unsigned int index = (vmi.XResolution * y + x) * bytes_per_pixel;
	
	if (memcpy(back_buffer + index, &color, bytes_per_pixel) == NULL) return 1;

	return 0;
}


int draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
	
	for (int i = 0; i < len; i++) {
		if (draw_pixel(x + i, y, color) != 0) {
			return 1;
		}
	}

	return 0;
}


int draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

	for (int i = 0; i < height; i++) {
		if (draw_hline(x, y + i, width, color) != 0) {
			return 1;
		}
	}

	return 0;
}

int draw_sprite(xpm_map_t xpm, int x, int y, uint32_t transparent_color) {
	xpm_image_t img;
	uint32_t *colors = (uint32_t*) xpm_load(xpm, XPM_8_8_8_8, &img);
	for (int i = 0; i < img.height; i++) {
		for (int j = 0; j < img.width; j++) {
			if ((*colors & 0x00FFFFFF) != (transparent_color & 0x00FFFFFF))
				draw_pixel(x + j, y + i, *colors);
			colors++;
		}
	}
	return 0;
}

int draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y) {

	xpm_image_t img;

	uint32_t *colors = (uint32_t*) xpm_load(xpm, XPM_8_8_8_8, &img);

	for (int i = 0 ; i < img.height ; i++) {
		for (int j = 0 ; j < img.width ; j++) {
			if (draw_pixel(x + j, y + i, *colors) != 0) {
				return 1;
			}
			colors++;
		}
	}
	return 0;
}


int draw_crosshair(int x, int y, int size, uint32_t color) {
	for (int i = -size; i <= size; i++)
		if (x + i >= 0) draw_pixel(x + i, y, color);
	for (int i = 1; i <= size; i++) {
		if (y - i >= 0) draw_pixel(x, y - i, color);
		draw_pixel(x, y + i, color);
	}
	return 0;
}

void vg_store_background() {
	memcpy(bg_buffer, back_buffer, vram_size);
}

void vg_restore_background() {
	memcpy(back_buffer, bg_buffer, vram_size);
}

void vg_flip() {
	memcpy(video_mem, back_buffer, vram_size);
}

void vg_free_buffer() {
	free(back_buffer);
	free(bg_buffer);
	back_buffer = NULL;
	bg_buffer = NULL;
}
