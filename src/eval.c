#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <eval.h>

int values[6] = {100, 500, 310, 330, 900, 100000};
static int pawn_mg_table[64] = {
	0,  0,	0,  0,	0,  0,	0,  0,
	5, 10, 10,-20,-20, 10, 10,  5,
	5, -5,-10,  0,	0,-10, -5,  5,
	0,  0,	0, 20, 20,  0,	0,  0,
	5,  5, 10, 25, 25, 10,	5,  5,
	10, 10, 20, 30, 30, 20, 10, 10,
	50, 50, 50, 50, 50, 50, 50, 50,
	0,  0,	0,  0,	0,  0,	0,  0
};
static int pawn_eg_table[64] = {
	0,   0,   0,   0,   0,	 0,   0,   0,
	5,   5,   5,   5,   5,	 5,   5,   5,
	10,  10,  10,  10,  10,  10,  10,  10,
	20,  20,  20,  20,  20,  20,  20,  20,
	35,  35,  35,  35,  35,  35,  35,  35,
	60,  60,  60,  60,  60,  60,  60,  60,
	100, 100, 100, 100, 100, 100, 100, 100,
	0,   0,   0,   0,   0,	 0,   0,   0
};
static int horse_mg_table[64] = {
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20, 0, 5, 5,  0, -20, -40,
	-30, 5, 10, 15, 15, 10,  5,-30,
	-30, 0, 15, 20, 20, 15,  0,-30,
	-30, 5, 15, 20, 20, 15,  5,-30,
	-30, 0, 10, 15, 15, 10,  0,-30,
	-40,-20, 0,  0,  0,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50
};
static int horse_eg_table[64] = {
	-40,-30,-20,-20,-20,-20,-30,-40,
	-30,-10,  0,  0,  0,  0,-10,-30,
	-20,  0, 10, 15, 15, 10,  0,-20,
	-20,  5, 15, 20, 20, 15,  5,-20,
	-20,  5, 15, 20, 20, 15,  5,-20,
	-20,  0, 10, 15, 15, 10,  0,-20,
	-30,-10,  0,  5,  5,  0,-10,-30,
	-40,-30,-20,-20,-20,-20,-30,-40
};
static int alfil_mg_table[64] = {
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10,-10,-10,-10,-10,-20
};
static int alfil_eg_table[64] = {
	-10,-5,-5,-5,-5,-5,-5,-10,
	-5, 5, 5, 5, 5, 5, 5, -5,
	-5,10,10,10,10,10,10, -5,
	-5, 5,10,10,10,10, 5, -5,
	-5, 5,10,10,10,10, 5, -5,
	-5,10,10,10,10,10,10, -5,
	-5, 5, 5, 5, 5, 5, 5, -5,
	-10,-5,-5,-5,-5,-5,-5,-10
};
static int rook_mg_table[64] = {
	0,  0,	5, 10, 10,  5,	0,  0,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	5, 10, 10, 10, 10, 10, 10,  5,
	0,  0,	0,  0,	0,  0,	0,  0
};
static int rook_eg_table[64] = {
	0,  0,	5, 10, 10,  5,	0,  0,
	5, 10, 10, 15, 15, 10, 10,  5,
	0,  5,	5, 10, 10,  5,	5,  0,
	0,  0,	5, 10, 10,  5,	0,  0,
	0,  0,	5, 10, 10,  5,	0,  0,
	0,  5,	5, 10, 10,  5,	5,  0,
	5, 10, 10, 15, 15, 10, 10,  5,
	0,  0,	5, 10, 10,  5,	0,  0
};
static int queen_mg_table[64] = {
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-10,  5,  5,  5,  5,  5,  0,-10,
	  0,  0,  5,  5,  5,  5,  0, -5,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
};
static int queen_eg_table[64] = {
	-10,-5,-5,-5,-5,-5,-5,-10,
	-5, 0, 5, 5, 5, 5, 0, -5,
	-5, 5, 5,10,10, 5, 5, -5,
	-5, 5,10,10,10,10, 5, -5,
	-5, 5,10,10,10,10, 5, -5,
	-5, 5, 5,10,10, 5, 5, -5,
	-5, 0, 5, 5, 5, 5, 0, -5,
	-10,-5,-5,-5,-5,-5,-5,-10
};
static int king_mg_table[64] = {
	20, 30, 10,  0,  0, 10, 30, 20,
	20, 20,  0,  0,  0,  0, 20, 20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30
};
static int king_eg_table[64] = {
	-50,-30,-30,-30,-30,-30,-30,-50,
	-30,-10,  0,  0,  0,  0,-10,-30,
	-30,  0, 20, 30, 30, 20,  0,-30,
	-30,  0, 30, 40, 40, 30,  0,-30,
	-30,  0, 30, 40, 40, 30,  0,-30,
	-30,  0, 20, 30, 30, 20,  0,-30,
	-30,-10,  0,  0,  0,  0,-10,-30,
	-50,-30,-30,-30,-30,-30,-30,-50
};

static int passed_mg[6] = {5, 10, 20, 35, 60, 100};
static int passed_eg[6] = {15, 25, 45, 70, 110, 170};
static int *tables_mg[6] = {pawn_mg_table, rook_mg_table, horse_mg_table, alfil_mg_table, queen_mg_table, king_mg_table};
static int *tables_eg[6] = {pawn_eg_table, rook_eg_table, horse_eg_table, alfil_eg_table, queen_eg_table, king_eg_table};
static int fase_values[6] = {0, 2, 1, 1, 4, 0};

static int passed_withe_bonus (int sq, uint64_t enemy_pawn, uint64_t friend_rook, uint64_t enemy_rook, int fase) {
	int col = sq % 8;
	int fila = sq >> 3;
	uint64_t front = 0;
	for (int i = sq + 8; i < 64; i += 8) front |= (1ULL << i);
	if (col > 0) for (int i = sq + 7; i < 64; i += 8) front |= (1ULL << i);
	if (col < 7) for (int i = sq + 9; i < 64; i += 8) front |= (1ULL << i);

	int bonus = 0;
	if (!(front & enemy_pawn)) {
		int idx = fila - 2;
		if (idx >= 0 && idx < 6) {
			bonus += (passed_mg[idx] * fase + passed_eg[idx] * (24 - fase)) / 24;
		}
		uint64_t col_behind = 0;
		for (int r = 0; r < fila; r++) col_behind |= (1ULL << (r * 8 + col));
		if (friend_rook & col_behind) bonus += (0 * fase + 20 * (24 - fase)) / 24;
		if (enemy_rook & col_behind) bonus -= (0 * fase + 15 * (24 - fase)) / 24;
	}	
	return bonus;
}

static int passed_black_bonus (int sq, uint64_t enemy_pawn, uint64_t friend_rook, uint64_t enemy_rook, int fase) {
	int col = sq % 8;
	int fila = sq >> 3;
	uint64_t front = 0;
	for (int i = sq - 8; i >= 0; i -= 8) front |= (1ULL << i);
	if (col > 0) for (int i = sq - 9; i >= 0; i -= 8) front |= (1ULL << i);
	if (col < 7) for (int i = sq - 7; i >= 0; i -= 8) front |= (1ULL << i);

	int bonus = 0;
	if (!(front & enemy_pawn)) {
		int idx = 5 - fila;
		if (idx >= 0 && idx < 6) {
			bonus += (passed_mg[idx] * fase + passed_eg[idx] * (24 - fase)) / 24;
		}
		uint64_t col_behind = 0;
		for (int r = fila + 1; r < 8; r++) col_behind |= (1ULL << (r * 8 + col));
		if (friend_rook & col_behind) bonus += (20 * (24 - fase)) / 24;
		if (enemy_rook & col_behind) bonus -= (15 * (24 - fase)) / 24;
	}
	return bonus;
}

int eval_pos (Pos *pos) {
	int eval = 0;
	int fase = 0;
	for (int p = 0; p < 6; p++) {
		int num_w = __builtin_popcountll(pos->bitboard[p]);
		int num_b = __builtin_popcountll(pos->bitboard[p + 6]);
		eval += (num_w - num_b) * values[p];
		fase += (num_w + num_b) * fase_values[p];
	}
	if (fase > 24) fase = 24;

	uint64_t all_pawns = pos->bitboard[0] | pos->bitboard[6];

	for (int p = 0; p < 6; p++) {
		uint64_t bits = pos->bitboard[p];
		while (bits) {
			int sq = __builtin_ctzll(bits);
			bits &= bits - 1;
			eval += (tables_mg[p][sq] * fase + tables_eg[p][sq] * (24 - fase)) / 24;
			if (!p) eval += passed_withe_bonus(sq, pos->bitboard[6], pos->bitboard[1], pos->bitboard[7], fase);
			if (p == 1) {
				uint64_t col = 0x0101010101010101ULL << (sq % 8);
				if (!(all_pawns & col)) eval += 20;
				else if (!(pos->bitboard[0] & col)) eval += 10;
			}
		}
		bits = pos->bitboard[p + 6];
		while (bits) {
			int sq_real = __builtin_ctzll(bits);
			int sq = sq_real ^ 56;
			bits &= bits - 1;
			eval -= (tables_mg[p][sq] * fase + tables_eg[p][sq] * (24 - fase)) / 24;
			if (!p) eval -= passed_black_bonus(sq_real, pos->bitboard[0], pos->bitboard[7], pos->bitboard[1], fase);
			if (p == 1) {
				uint64_t col = 0x0101010101010101ULL << (sq % 8);
				if (!(all_pawns & col)) eval -= 20;
				else if (!(pos->bitboard[6] & col)) eval -= 10;
			}
		}
	}

	if (__builtin_popcountll(pos->bitboard[3]) >= 2) eval += 30;
	if (__builtin_popcountll(pos->bitboard[9]) >= 2) eval -= 30;

	uint64_t col_mask;
	for (int i = 0; i < 8; i++) {
		col_mask = 0x0101010101010101ULL << i;
		int w = __builtin_popcountll(pos->bitboard[0] & col_mask);
    		int b = __builtin_popcountll(pos->bitboard[6] & col_mask);

    		if (w > 1) eval -= (w - 1) * 15;
    		if (b > 1) eval += (b - 1) * 15;
	}
	
	return eval;
}
