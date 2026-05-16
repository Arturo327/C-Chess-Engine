#ifndef SORT_H
#define SORT_H

#include <stdint.h>

#include "types.h"

#define MAX_DEPTH 30

extern int history[12][64];
extern Move killers[MAX_DEPTH][2];

void sort_moves_quiescence (Move *moves, int count, Pos *pos, int *see_scores);
void sort_moves (Move *moves, int count, int ply, Pos *pos);

#endif
