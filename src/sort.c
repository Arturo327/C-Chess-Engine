#include "sort.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int score_move(Move *m) {
	int values[12] = {100, 500, 300, 300, 900, 100000, -100, -500, -300, -300, -900, -100000};
    	int score = 0;
	if (m->capture != -1) {
		score = abs(values[m->capture]) * 10 - abs(values[m->pieza]);
	}

	if (m->pieza == 0 && m->to > 55) score += 100;
	else if (m->pieza == 6 && m->to < 8) score += 100;
	else if ((m->pieza == 5 || m->pieza == 11) && abs(m->from - m->to) == 2) score += 50;
    	return score;
}

void sort_moves(Move *moves, int count) {
	for (int i = 0; i < count - 1; i++) {
		int best = i;
		int best_score = score_move(&moves[i]);
		for (int j = i + 1; j < count; j++) {
			int act_score = score_move(&moves[j]);
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

