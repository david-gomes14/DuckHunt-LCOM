#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "../../rtc/rtc.h"

#define MAX_SCORES 5

typedef struct {
    int score;
    rtc_date_t date;
} leaderboard_entry_t;

void leaderboard_init();
void leaderboard_add(int score);
leaderboard_entry_t *leaderboard_get();
int leaderboard_size();

#endif
