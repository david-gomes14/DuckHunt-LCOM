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

#define SCORE_X 250
#define SCORE_Y 20
#define TIME_X 1050
#define TIME_Y 20

#define DIGIT_SPACING 40

static xpm_map_t digits[] = {
    (xpm_map_t) zero_xpm,
    (xpm_map_t) one_xpm,
    (xpm_map_t) two_xpm,
    (xpm_map_t) three_xpm,
    (xpm_map_t) four_xpm,
    (xpm_map_t) five_xpm,
    (xpm_map_t) six_xpm,
    (xpm_map_t) seven_xpm,
    (xpm_map_t) eight_xpm,
    (xpm_map_t) nine_xpm
};

static void draw_number(int number, int x, int y) {

    if (number < 0 || number > 9)
        return;

    draw_sprite(digits[number], x, y, 0x000000);
}

void hud_draw(int score, int time_left) {

    int score_tens = score / 10;
    int score_units = score % 10;

    draw_number(score_tens, SCORE_X, SCORE_Y);
    draw_number(score_units, SCORE_X + DIGIT_SPACING, SCORE_Y);

    int time_tens = time_left / 10;
    int time_units = time_left % 10;

    draw_number(time_tens, TIME_X, TIME_Y);
    draw_number(time_units, TIME_X + DIGIT_SPACING, TIME_Y);
}

