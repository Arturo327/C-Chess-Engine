#include "motor.h"
#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static void get_move (char *s, Pos *pos) {
	int from = (s[0] - 'a') + (s[1] - '1') * 8;
	int to	 = (s[2] - 'a') + (s[3] - '1') * 8;
	Move m;
	m.from = from;
	m.to = to;
	m.pieza = pos->board[from];
	m.capture = pos->board[to];
	apply_move(&m, pos);
}

static int parse_position(const char *line, Pos *pos) {
	reset(pos);
	int move_count = 0;
	char *moves_ptr = strstr(line, " moves ");
	if (moves_ptr) {
		moves_ptr += 7;
		char *p = moves_ptr;
		while (*p && *p != '\n') {
			if (p[0] >= 'a' && p[0] <= 'h' && p[2] >= 'a' && p[2] <= 'h') {
				get_move(p, pos);
				move_count++;
			}
			while (*p && *p != ' ' && *p != '\n') p++;
			while (*p == ' ') p++;
		}
	}
	return move_count;
}

int main () {
	generate_horse_table();
	generate_king_table();
	init_zobrist();
	tt_clear();   
	Pos pos;
	reset(&pos);
	char line[4096];
	int num_moves = 0;
	while (fgets(line, sizeof(line), stdin)) {
		if (strncmp(line, "uci", 3) == 0) {
			printf("id name R2Chess\n");
			printf("id author Arturo327\n");
			printf("uciok\n");
			fflush(stdout);

		} else if (strncmp(line, "isready", 7) == 0) {
			printf("readyok\n");
			fflush(stdout);

		} else if (strncmp(line, "ucinewgame", 10) == 0) {
			reset(&pos);
			tt_clear();
			memset(history, 0, sizeof(history));
			memset(killers, 0, sizeof(killers));

		} else if (strncmp(line, "position", 8) == 0) {
			num_moves = parse_position(line, &pos);

		} else if (strncmp(line, "go", 2) == 0) {
			int depth = 10;

			Move best = bot_move(depth, &pos);
			printf("bestmove %c%c%c%c\n",
				'a' + (best.from & 7), '1' + (best.from >> 3),
				'a' + (best.to	 & 7), '1' + (best.to	>> 3));
			fflush(stdout);

		} else if (strncmp(line, "quit", 4) == 0) {
			break;
		}
	}

	return 0;
}

