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

	reg.intno = 0x10;  // interrupção BIOS de vídeo
	reg.ah = 0x4F;     // VBE
	reg.al = 0x02;     // função "set mode"
	reg.bx = mode | BIT(14);  // BIT(14) ativa linear frame buffer

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
	vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel;  // ~3.8 MB para 1152x864x32bit

	// dá permissão ao processo para aceder à memória física da VRAM
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
		printf("sys_privctl (ADD_MEM) failed: %d\n", r);
		return 1;
	}

	// mapeia a VRAM física no espaço de endereçamento virtual do processo
	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
	if (video_mem == MAP_FAILED) {
		printf("couldn't map video memory");
		return 1;
	}

	// back_buffer: frame atual em RAM (todas as funções draw_* escrevem aqui)
	back_buffer = malloc(vram_size);
	if (back_buffer == NULL) {
		printf("couldn't allocate back buffer\n");
		return 1;
	}
	memset(back_buffer, 0, vram_size);

	// bg_buffer: snapshot do fundo limpo; restaurado no início de cada frame
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


int draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y) {
	xpm_image_t img;
	uint32_t *colors = (uint32_t*) xpm_load(xpm, XPM_8_8_8_8, &img);
	draw_sprite(colors, &img, x, y, false);
	return 0;
}


void draw_sprite(uint32_t *pixels, xpm_image_t *info, int x, int y, bool facing_left) {
    for (int i = 0; i < info->height; i++) {
        for (int j = 0; j < info->width; j++) {
            // se facing_left, lê as colunas ao contrário para flip horizontal
            int src_j = facing_left ? (info->width - 1 - j) : j;
            uint32_t c = pixels[i * info->width + src_j];
            // alpha == 0 significa pixel com cor; alpha != 0 é transparente ("None" no XPM)
            if ((c >> 24) == 0)
                draw_pixel(x + j, y + i, c);
        }
    }
}


// só desenha se dentro do ecrã (guarda de negativos, que o draw_pixel não tem)
static void ch_plot(int x, int y, uint32_t color) {
	if (x >= 0 && y >= 0) draw_pixel((uint16_t) x, (uint16_t) y, color);
}

// quadrado cheio centrado em (cx,cy) com "raio" r (lado = 2r+1)
static void ch_fill(int cx, int cy, int r, uint32_t color) {
	for (int dy = -r; dy <= r; dy++)
		for (int dx = -r; dx <= r; dx++)
			ch_plot(cx + dx, cy + dy, color);
}

// mira estilo reticle: anel + cruz com gap + ticks + ponto central
int draw_crosshair(int x, int y, int size, uint32_t color) {
	int R = size;                         // raio do anel
	int gi = R / 2;                       // raio interior (gap no centro)
	int go = R + (size / 3 > 3 ? size / 3 : 3);  // estende-se para lá do anel (ticks)
	int t = 1;                            // metade da espessura dos braços (espessura 2t+1 = 3)

	// 1) ponto central (3x3)
	ch_fill(x, y, 1, color);

	// 2) anel de 2px (raio R e R-1), midpoint circle
	for (int rr = R - 1; rr <= R; rr++) {
		int cx = 0, cy = rr, d = 1 - rr;
		while (cx <= cy) {
			ch_plot(x + cx, y + cy, color); ch_plot(x - cx, y + cy, color);
			ch_plot(x + cx, y - cy, color); ch_plot(x - cx, y - cy, color);
			ch_plot(x + cy, y + cx, color); ch_plot(x - cy, y + cx, color);
			ch_plot(x + cy, y - cx, color); ch_plot(x - cy, y - cx, color);
			cx++;
			if (d < 0) d += 2 * cx + 1;
			else { cy--; d += 2 * (cx - cy) + 1; }
		}
	}

	// 3) quatro braços (espessura 3), de gi até go com gap no centro
	for (int i = gi; i <= go; i++)
		for (int w = -t; w <= t; w++) {
			ch_plot(x + i, y + w, color);   // direita
			ch_plot(x - i, y + w, color);   // esquerda
			ch_plot(x + w, y + i, color);   // baixo
			ch_plot(x + w, y - i, color);   // cima
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
