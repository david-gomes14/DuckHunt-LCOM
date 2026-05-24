#include <lcom/lcf.h>

#include "score.h"

#include "../../graphic/graphic.h"
#include "../state/state.h"

static int score = 0;

void score_reset() {
    score = 0;
}

void score_add(int points) {
    score += points;
}

int score_get() {
    return score;
}
