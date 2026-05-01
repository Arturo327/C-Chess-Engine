#include "sort.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int history[12][64] = {0};
Move killers[MAX_DEPTH][2] = {0};

int score_move(Move *m, int ply) {
	if (m->capture != -1) {
		int values[12] = {100, 500, 300, 300, 900, 1000000, 100, 500, 300, 300, 900, 1000000};
		return values[m->capture] * 10 - values[m->pieza] + 2000000;
	}

	if ((m->pieza == 0 && m->to >= 56) || (m->pieza == 6 && m->to <= 7)) return 1900000;

	if (killers[ply][0].from == m->from && killers[ply][0].to == m->to) return 900000;

	if (killers[ply][1].from == m->from && killers[ply][1].to == m->to) return 800000;

	return history[m->pieza][m->to];
}

void sort_moves(Move *moves, int count, int ply) {
	for (int i = 0; i < count - 1; i++) {
		int best = i;
		int best_score = score_move(&moves[i], ply);
		for (int j = i + 1; j < count; j++) {
			int act_score = score_move(&moves[j], ply);
			if (act_score > best_score) {
				best = j;
				best_score = act_score;
			}
		}
		if (best != i) {
			Move tmp = moves[i];
			moves[i] = moves[best];
			moves[best] = tmp;
		}
	}
}

