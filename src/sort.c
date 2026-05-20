#include "sort.h"
#include "see.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int history[12][64] = {0};
Move killers[MAX_DEPTH][2] = {0};

static int score_move(Move *m, int ply, Pos *pos) {
	if (m->capture != -1) {
		int see_val = see(pos, m->to, values[m->capture % 6], m->from, values[m->pieza % 6]);
		if (see_val >= 0) {
			return 2000000 + see_val;
		} else {
			return 1000000 + see_val;
		}
	}

	if (m->to >= 56 && m->from >= 48) {
		if (m->pieza == 4) return 1900000;
		if (m->pieza == 1) return 1500000;
		if (m->pieza == 3) return 1400000;
		if (m->pieza == 2) return 1600000;
	}

	if (m->to <= 7 && m->from <= 15) {
		if (m->pieza == 10) return 1900000;
		if (m->pieza == 7) return 1500000;
		if (m->pieza == 9) return 1400000;
		if (m->pieza == 8) return 1600000;
	}

	if (killers[ply][0].from == m->from && killers[ply][0].to == m->to) return 900000;

	if (killers[ply][1].from == m->from && killers[ply][1].to == m->to) return 800000;

	return history[m->pieza][m->to];
}

void sort_moves_quiescence (Move *moves, int count, int *see_scores) {
	int scores[count];
	for (int i = 0; i < count; i++)
		if (moves[i].capture != -1) scores[i] = see_scores[i] + 2000000;
		else scores[i] = history[moves[i].pieza][moves[i].to];

	for (int i = 0; i < count - 1; i++) {
		int best = i;
		int best_score = scores[i];
		for (int j = i + 1; j < count; j++) {
			int act_score = scores[j];
			if (act_score > best_score) {
				best = j;
				best_score = act_score;
			}
		}
		if (best != i) {
			Move tmp = moves[i];
			moves[i] = moves[best];
			moves[best] = tmp;

			int stmp = scores[i];
			scores[i] = scores[best];
			scores[best] = stmp;

			int stmp_see = see_scores[i];
			see_scores[i] = see_scores[best];
			see_scores[best] = stmp_see;
		}
	}
}

void sort_moves (Move *moves, int count, int ply, Pos *pos) {
	int scores[count];
	for (int i = 0; i < count; i++) 
		scores[i] = score_move(&moves[i], ply, pos);

	for (int i = 0; i < count - 1; i++) {
		Move key = moves[i];
		int key_score = scores[i];
		int j = i - 1;
		while (j >= 0 && scores[j] < key_score) {
			moves[j + 1] = moves[j];
			scores[j + 1] = scores[j];
			j--;
		}
		moves[j + 1] = key;
		scores[j + 1] = key_score;
	}
}

