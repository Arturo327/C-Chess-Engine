#include "motor.h"
#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static void get_move (char *s, Pos *pos) {
	int from = (s[0] - 'a') + (s[1] - '1') * 8;
	int to = (s[2] - 'a') + (s[3] - '1') * 8;
	int side = pos->side;
	Move m;
	m.from = from;
	m.to = to;
	m.pieza = pos->board[from];
	if (strlen(s) > 4) {
		switch (s[4]) {
			case 'q':
				m.pieza = side ? 10 : 4;
				break;
			case 'r':
				m.pieza = side ? 7 : 1;
				break;
			case 'b':
				m.pieza = side ? 9 : 3;
				break;
			case 'n':
				m.pieza = side ? 8 : 2;
				break;
		}
	}
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
			int depth = 16;
			Move best = bot_move(depth, &pos);
			printf("bestmove %c%c%c%c",
				'a' + (best.from & 7), '1' + (best.from >> 3),
				'a' + (best.to	 & 7), '1' + (best.to	>> 3));

			char promo_char = 0;
			if (pos.board[best.from] == 0 && best.pieza >= 1 && best.pieza <= 4) {
				char pc[] = {0, 'r', 'n', 'b', 'q'};
				promo_char = pc[best.pieza];
			}
			if (pos.board[best.from] == 6 && best.pieza >= 7 && best.pieza <= 10) {
				char pc[] = {0,0,0,0,0,0,0,'r','n','b','q'};
				promo_char = pc[best.pieza];
			}
			if (promo_char) printf("%c\n", promo_char);
			else printf("\n");
			fflush(stdout);

		} else if (strncmp(line, "quit", 4) == 0) {
			break;
		}
	}

	return 0;
}

