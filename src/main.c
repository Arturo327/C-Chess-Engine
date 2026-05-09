#include "motor.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static int char_to_piece(char c) {
	switch (c) {
		case 'P': return 0;
		case 'R': return 1;
		case 'N': return 2;
		case 'B': return 3;
		case 'Q': return 4;
		case 'K': return 5;
		case 'p': return 6;
		case 'r': return 7;
		case 'n': return 8;
		case 'b': return 9;
		case 'q': return 10;
		case 'k': return 11;
	}
	return -1;
}

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
	int move_count = 0;
	char *start = strstr(line, " startpos");
	if (start) {
		reset(pos);
	} else {
		int sq = 56;
		char *position = strstr(line, " fen ");
		if (!position) return move_count;
		position += 5;
		char *p = position;
		memset(pos->board, 0xFF, 64);
		memset(pos->bitboard, 0, 12 * sizeof(uint64_t));
		while (*p && *p != ' ') {
			if (*p == '/') {
				sq -= 16;
			} 
			else if (*p >= '1' && *p <= '8') {
				sq += *p - '0';
			} 
			else {
				int pieza = char_to_piece(*p);
				pos->board[sq] = pieza;
				pos->bitboard[pieza] |= (1ULL << sq);
				sq++;
			}
			p++;
		}
		p++;
		pos->side = (*p == 'w') ? 0 : 1;
		p += 2;
		pos->castle = 0;
		while (*p && *p != ' ') {
			switch (*p) {
				case 'K': pos->castle |= 1; break;
				case 'Q': pos->castle |= 2; break;
				case 'k': pos->castle |= 4; break;
				case 'q': pos->castle |= 8; break;
			}
			p++;
		}
		p++;
		if (*p == '-') {
			pos->en_passant = -1;
		} else {
			int file = p[0] - 'a';
			int rank = p[1] - '1';
			pos->en_passant = file + rank * 8;
		}
		pos->hash = compute_hash(pos);
	}

	rep_top = 0;
	rep_stack[rep_top++] = pos->hash;

	char *moves_ptr = strstr(line, " moves ");
	if (moves_ptr) {
		moves_ptr += 7;
		char *p = moves_ptr;
		while (*p && *p != '\n') {
			if (p[0] >= 'a' && p[0] <= 'h' && p[2] >= 'a' && p[2] <= 'h') {
				get_move(p, pos);
				rep_stack[rep_top++] = pos->hash;
				move_count++;
			}
			while (*p && *p != ' ' && *p != '\n') p++;
			while (*p == ' ') p++;
		}
	} 
	return move_count;
}

int main (int argc, char *argv[]) {
	generate_horse_table();
	generate_king_table();
	init_magics();
	init_zobrist();
	tt_clear();   
	Pos pos;
	reset(&pos);
	char line[4096];
	int num_moves = 0;
	while (fgets(line, sizeof(line), stdin)) {
		if (strncmp(line, "ucinewgame", 10) == 0) {
			reset(&pos);
			tt_clear();
			memset(history, 0, sizeof(history));
			memset(killers, 0, sizeof(killers));

		} else if (strncmp(line, "uci", 3) == 0) {
			printf("id name R2Chess\n");
			printf("id author Arturo327\n");
			printf("uciok\n");
			fflush(stdout);

		} else if (strncmp(line, "isready", 7) == 0) {
			printf("readyok\n");
			fflush(stdout);

		} else if (strncmp(line, "position", 8) == 0) {
			num_moves = parse_position(line, &pos);

		} else if (strncmp(line, "go", 2) == 0) {
			int depth = 20;
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

