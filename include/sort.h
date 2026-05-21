#ifndef SORT_H
#define SORT_H

#include <stdint.h>

#include "types.h"

#define MAX_DEPTH 30

extern int history[12][64];
extern Move killers[MAX_DEPTH][2];

int score_move (Move *m, int ply);
void sort_moves (Move *moves, int count, int *scores);

#endif
