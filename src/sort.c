#include "sort.h"
#include "see.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int history[12][64] = {0};
Move killers[MAX_DEPTH][2] = {0};

static int mvv_lva[6][6] = {
	{ 15, 14, 13, 12, 11, 10 },
	{ 25, 24, 23, 22, 21, 20 },
	{ 35, 34, 33, 32, 31, 30 },
	{ 45, 44, 43, 42, 41, 40 },
	{ 55, 54, 53, 52, 51, 50 },
	{ 65, 64, 63, 62, 61, 60 },
};

int score_move (Move *m, int ply) {
	if (m->capture != -1) {
		int victim   = m->capture % 6;
		int attacker = m->pieza % 6;
		return 2000000 + mvv_lva[victim][attacker];
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

void sort_moves (Move *moves, int count, int *scores) {
	for (int i = 1; i < count; i++) {
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

