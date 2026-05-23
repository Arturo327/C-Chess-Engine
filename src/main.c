#include "motor.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define FEN_FILE "files/bench.fen"

static int char_to_piece (char c) {
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

static void get_fen (const char *line, Pos *pos) {
	int sq = 56;
	const char *p = line;
	memset(pos->board, ~0, 64 * sizeof(int));
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
	pos->occupied = 0;
	for (int i = 0; i < 12; i++) pos->occupied |= pos->bitboard[i];
}

static void parse_position (const char *line, Pos *pos) {
	char *start = strstr(line, " startpos");
	if (start) {
		reset(pos);
	} else {
		char *position = strstr(line, " fen ");
		if (!position) return;
		get_fen (position + 5, pos);
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
			}
			while (*p && *p != ' ' && *p != '\n') p++;
			while (*p == ' ') p++;
		}
	} 
}

static void bench (const char *filename) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		fprintf(stderr, "bench: no se puede abrir %s\n", filename);
		return;
	}

	long long total_nodes = 0;
	long long total_time = 0;
	int positions = 0;
	int depth = 14;
	int correct = 0;

	char line[1024];
	while (fgets(line, sizeof(line), f)) {
		if (line[0] == '\n' || line[0] == '#') continue;
		line[strcspn(line, "\n")] = 0;
		Pos pos;
		get_fen (line, &pos);

		char *p = strstr(line, " bm ");
		char best[7] = {0};
		if (p != NULL) {
			p += 4;
			(void)sscanf(p, "%7[^;\n]", best);
		}

		correct += bench_pos (depth, &pos, &total_time, &total_nodes, best);
		positions++;
	}

	fclose(f);

	if (positions == 0) {
		printf("bench: no se encontraron posiciones\n");
		return;
	}

	if (total_time == 0) total_time = 1;
	long long avg_nps = total_nodes * 1000LL / total_time;
	printf("\n=== BENCH RESULTADO ===\n");
	printf("Posiciones : %d\n", positions);
	printf("Nodos total: %lld\n", total_nodes);
	printf("Tiempo total: %lldms\n", total_time);
	printf("NPS medio  : %lld\n", avg_nps);
	printf("Aciertos   : %d/30\n", correct);
}

static long long get_movetime (char *line, int side) {
	long long wtime = -1, btime = -1, winc = 0, binc = 0;
	long long movetime = -1, movestogo = 0;

	char *p = line + 2;
	while (*p) {
		if (strncmp(p, " wtime ", 7) == 0) { p += 7; wtime = atoll(p); }
		else if (strncmp(p, " btime ", 7) == 0) { p += 7; btime = atoll(p); }
		else if (strncmp(p, " winc ", 6) == 0) { p += 6; winc = atoll(p); }
		else if (strncmp(p, " binc ", 6) == 0) { p += 6; binc = atoll(p); }
		else if (strncmp(p, " movetime ", 10) == 0) { p += 10; movetime = atoll(p); }
		else if (strncmp(p, " movestogo ", 11) == 0)  { p += 11; movestogo = atoll(p); }
		p++;
	}

	long long time_for_move;
	if (movetime > 0) {
		time_for_move = movetime - 50;
	} else if (wtime > 0 || btime > 0) {
		long long my_time = (side == 0) ? wtime : btime;
		long long my_inc = (side == 0) ? winc : binc;
		if (my_time < 0) my_time = 10000;

		if (movestogo > 0) {
			time_for_move = my_time / (movestogo + 2) + my_inc * 8 / 10;
		} else {
			time_for_move = my_time / 25 + my_inc * 8 / 10;
		}
		if (time_for_move > my_time * 8 / 10) time_for_move = my_time * 8 / 10;
		if (time_for_move < 10) time_for_move = 10;
	} else time_for_move = 600000;
	return time_for_move;
}

int main (void) {
	generate_horse_table();
	generate_king_table();
	init_magics();
	init_zobrist();
	tt_clear();   
	reset_lmr();
	reset_eval_masks();

	Pos pos;
	reset(&pos);
	char line[4096];
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
			parse_position(line, &pos);

		} else if (strncmp(line, "bench", 5) == 0) {
			bench(FEN_FILE);

		} else if (strncmp(line, "go", 2) == 0) {
			int depth = 30;
			long long movetime = get_movetime(line, pos.side);
			char *a = strstr(line, " depth ");
			if (a) {
				a += 7;
				depth = atoi(a);
			}
			Move best = bot_move(depth, movetime, &pos);
			printf("bestmove %c%c%c%c",
				'a' + (best.from & 7), '1' + (best.from >> 3),
				'a' + (best.to & 7), '1' + (best.to >> 3));

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

