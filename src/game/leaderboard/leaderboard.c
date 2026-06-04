#include <lcom/lcf.h>

#include "leaderboard.h"

static leaderboard_entry_t leaderboard[MAX_SCORES];

static int num_entries = 0;

void leaderboard_init() {
    num_entries = 0;
}

void leaderboard_add(int score) {
    leaderboard_entry_t entry;
    entry.score = score;

    if (rtc_read_date(&entry.date) != 0) return;

    if (num_entries < MAX_SCORES) {
        leaderboard[num_entries++] = entry;
    } else {
        if (score <= leaderboard[MAX_SCORES - 1].score) return;
        leaderboard[MAX_SCORES - 1] = entry;
    }

    for (int i = 0; i < num_entries - 1; i++) {
        for (int j = i + 1; j < num_entries; j++) {
            if (leaderboard[j].score > leaderboard[i].score) {
                leaderboard_entry_t tmp = leaderboard[i];
                leaderboard[i] = leaderboard[j];
                leaderboard[j] = tmp;
            }
        }
    }
}

int leaderboard_size() {
    return num_entries;
}

leaderboard_entry_t *leaderboard_get() {
    return leaderboard;
}
