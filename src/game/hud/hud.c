#include <lcom/lcf.h>

#include "hud.h"

#include "../../graphic/graphic.h"

#include "../../images/numbers/zero.xpm"
#include "../../images/numbers/one.xpm"
#include "../../images/numbers/two.xpm"
#include "../../images/numbers/three.xpm"
#include "../../images/numbers/four.xpm"
#include "../../images/numbers/five.xpm"
#include "../../images/numbers/six.xpm"
#include "../../images/numbers/seven.xpm"
#include "../../images/numbers/eight.xpm"
#include "../../images/numbers/nine.xpm"

#include "../../images/numbers/zero_lg.xpm"
#include "../../images/numbers/one_lg.xpm"
#include "../../images/numbers/two_lg.xpm"
#include "../../images/numbers/three_lg.xpm"
#include "../../images/numbers/four_lg.xpm"
#include "../../images/numbers/five_lg.xpm"
#include "../../images/numbers/six_lg.xpm"
#include "../../images/numbers/seven_lg.xpm"
#include "../../images/numbers/eight_lg.xpm"
#include "../../images/numbers/nine_lg.xpm"

#define SCORE_X 200
#define SCORE_Y 55
#define TIME_X 940
#define TIME_Y 65

#define DIGIT_SPACING 36

static xpm_map_t digit_maps[] = {
    (xpm_map_t) zero_xpm,  (xpm_map_t) one_xpm,   (xpm_map_t) two_xpm,
    (xpm_map_t) three_xpm, (xpm_map_t) four_xpm,  (xpm_map_t) five_xpm,
    (xpm_map_t) six_xpm,   (xpm_map_t) seven_xpm, (xpm_map_t) eight_xpm,
    (xpm_map_t) nine_xpm
};

static uint32_t *digit_pixels[10];
static xpm_image_t digit_info[10];

static xpm_map_t digit_maps_lg[] = {
    (xpm_map_t) zero_lg_xpm,  (xpm_map_t) one_lg_xpm,   (xpm_map_t) two_lg_xpm,
    (xpm_map_t) three_lg_xpm, (xpm_map_t) four_lg_xpm,  (xpm_map_t) five_lg_xpm,
    (xpm_map_t) six_lg_xpm,   (xpm_map_t) seven_lg_xpm, (xpm_map_t) eight_lg_xpm,
    (xpm_map_t) nine_lg_xpm
};

static uint32_t *digit_pixels_lg[10];
static xpm_image_t digit_info_lg[10];

void hud_init() {
    if (digit_pixels[0] != NULL) return;
    for (int i = 0; i < 10; i++) {
        digit_pixels[i]    = (uint32_t *) xpm_load(digit_maps[i],    XPM_8_8_8_8, &digit_info[i]);
        digit_pixels_lg[i] = (uint32_t *) xpm_load(digit_maps_lg[i], XPM_8_8_8_8, &digit_info_lg[i]);
    }
}

static void draw_number(int n, int x, int y) {
    if (n < 0 || n > 9) return;
    xpm_image_t *info = &digit_info[n];
    uint32_t *px = digit_pixels[n];
    for (int i = 0; i < info->height; i++)
        for (int j = 0; j < info->width; j++) {
            uint32_t c = px[i * info->width + j];
            // fundo preto é transparente (chroma key)
            if ((c & 0x00FFFFFF) != 0x000000)
                draw_pixel(x + j, y + i, c);
        }
}

static void draw_value(int val, int x, int y) {
    val %= 1000;
    int hundreds = val / 100;
    int tens     = (val / 10) % 10;
    int units    = val % 10;
    if (hundreds > 0)
        draw_number(hundreds, x,                     y);
    draw_number(tens,         x + DIGIT_SPACING,     y);
    draw_number(units,        x + DIGIT_SPACING * 2, y);
}

void hud_draw(int score, int time_left) {
    draw_value(score,     SCORE_X, SCORE_Y);
    draw_value(time_left, TIME_X,  TIME_Y);
}

void hud_draw_score_at(int score, int x, int y) {
    draw_value(score, x, y);
}

void hud_draw_number_at(int n, int x, int y) {
    draw_number(n, x, y);
}

// desenha "HH:MM | DD-MM" a partir de x,y — separadores a dourado.
// barra vertical separa hora da data. largura total ≈ 372px
void hud_draw_datetime_at(uint8_t hour, uint8_t minute, uint8_t day, uint8_t month, int x, int y) {
    #define SEP_COLOR 0xFFB800

    // HH
    draw_number(hour   / 10, x,                  y);
    draw_number(hour   % 10, x + DIGIT_SPACING,  y);
    // : (dois quadrados 4x4), centrado verticalmente ao lado dos dígitos
    int cx = x + DIGIT_SPACING + 42;
    draw_rectangle(cx, y + 19, 4, 4, SEP_COLOR);
    draw_rectangle(cx, y + 33, 4, 4, SEP_COLOR);
    // MM
    int mm_x = cx + 12;
    draw_number(minute / 10, mm_x,                 y);
    draw_number(minute % 10, mm_x + DIGIT_SPACING, y);
    // separador vertical entre hora e data (barra dourada 4×26)
    int bar_x = mm_x + DIGIT_SPACING + 50;
    draw_rectangle(bar_x, y + 12, 4, 26, SEP_COLOR);
    // DD
    int date_x = bar_x + 18;
    draw_number(day   / 10, date_x,                 y);
    draw_number(day   % 10, date_x + DIGIT_SPACING, y);
    // - (rectângulo horizontal 10x4)
    int dash_x = date_x + DIGIT_SPACING + 44;
    draw_rectangle(dash_x, y + 23, 10, 4, SEP_COLOR);
    // MM (mês)
    int mon_x = dash_x + 16;
    draw_number(month / 10, mon_x,                 y);
    draw_number(month % 10, mon_x + DIGIT_SPACING, y);

    #undef SEP_COLOR
}

void hud_draw_score_large_at(int score, int x, int y) {
    score %= 1000;
    int hundreds = score / 100;
    int tens     = (score / 10) % 10;
    int units    = score % 10;

    // reutiliza draw_number mas com os pixels grandes
    // (inline para não precisar de refatorar draw_number)
    uint32_t **px_arr = digit_pixels_lg;
    xpm_image_t *info_arr = digit_info_lg;
    int spacing = 74;

    int digits[3] = {hundreds, tens, units};
    int start = (hundreds > 0) ? 0 : 1;
    for (int d = start; d < 3; d++) {
        int n = digits[d];
        xpm_image_t *info = &info_arr[n];
        uint32_t *px = px_arr[n];
        int dx = x + (d - 1) * spacing;
        for (int i = 0; i < info->height; i++)
            for (int j = 0; j < info->width; j++) {
                uint32_t c = px[i * info->width + j];
                if ((c & 0x00FFFFFF) != 0x000000)
                    draw_pixel(dx + j, y + i, c);
            }
    }
}

