#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "movegen.h"
#include "make_move.h"

int is_attacked(int sq, int by_white, Pos *pos) {
	uint64_t friendly = by_white ?
		pos->bitboard[0]|pos->bitboard[1]|pos->bitboard[2]|pos->bitboard[3]|pos->bitboard[4]|pos->bitboard[5] :
		pos->bitboard[6]|pos->bitboard[7]|pos->bitboard[8]|pos->bitboard[9]|pos->bitboard[10]|pos->bitboard[11];
	uint64_t enemy = by_white ?
		pos->bitboard[6]|pos->bitboard[7]|pos->bitboard[8]|pos->bitboard[9]|pos->bitboard[10]|pos->bitboard[11] :
		pos->bitboard[0]|pos->bitboard[1]|pos->bitboard[2]|pos->bitboard[3]|pos->bitboard[4]|pos->bitboard[5];

	uint64_t knightatt = by_white ? pos->bitboard[2] : pos->bitboard[8];
	if (horse_moves[sq] & knightatt) return 1;
	uint64_t kingatt = by_white ? pos->bitboard[5] : pos->bitboard[11];
	if (king_moves[sq] & kingatt) return 1;
	uint64_t rookatt = by_white ? pos->bitboard[1]|pos->bitboard[4] : pos->bitboard[7]|pos->bitboard[10];
	if (get_rook_attacks(sq, enemy, friendly) & rookatt) return 1;
	uint64_t bishatt = by_white ? pos->bitboard[3]|pos->bitboard[4] : pos->bitboard[9]|pos->bitboard[10];
	if (get_alfil_attacks(sq, enemy, friendly) & bishatt) return 1;
	if (by_white) {
		uint64_t col_a = 0x0101010101010101ULL;
		uint64_t col_h = 0x8080808080808080ULL;
		uint64_t bit = 1ULL << sq;
		if ((bit >> 7) & pos->bitboard[0] & ~col_a) return 1;
		if ((bit >> 9) & pos->bitboard[0] & ~col_h) return 1;
	} else {
		uint64_t col_a = 0x0101010101010101ULL;
		uint64_t col_h = 0x8080808080808080ULL;
		uint64_t bit = 1ULL << sq;
		if ((bit << 7) & pos->bitboard[6] & ~col_h) return 1;
		if ((bit << 9) & pos->bitboard[6] & ~col_a) return 1;
	}
	return 0;
}

uint64_t get_w_pawn_moves (int from, uint64_t occupied) {
	uint64_t attacks = 0;
	uint64_t pawn = 1ULL << from;
	uint64_t push = (pawn << 8) & ~occupied;
	attacks |= push;
	attacks |= (push << 8) & ~occupied & 0x00000000FF000000ULL;
	return attacks;
}

uint64_t get_w_pawn_captures (int from, uint64_t enemy, int en_passant) {
	uint64_t attacks = 0;
	uint64_t pawn = 1ULL << from;
	uint64_t col_a = 0x0101010101010101ULL;
	uint64_t col_h = 0x8080808080808080ULL;
	attacks |= (pawn << 9) & enemy & ~col_a;
	attacks |= (pawn << 7) & enemy & ~col_h;
	if (en_passant != -1) {
		uint64_t ep = 1ULL << en_passant;
		if ((pawn << 9) & ep & ~col_a) attacks |= ep;
		if ((pawn << 7) & ep & ~col_h) attacks |= ep;
	}
	return attacks;
}

uint64_t get_b_pawn_moves (int from, uint64_t occupied) {
	uint64_t attacks = 0;
	uint64_t pawn = 1ULL << from;
	uint64_t push = (pawn >> 8) & ~occupied;
	attacks |= push;
	attacks |= (push >> 8) & ~occupied & 0x000000FF00000000ULL;
	return attacks;
}

uint64_t get_b_pawn_captures (int from, uint64_t enemy, int en_passant) {
	uint64_t attacks = 0;
	uint64_t pawn = 1ULL << from;
	uint64_t col_a = 0x0101010101010101ULL;
	uint64_t col_h = 0x8080808080808080ULL;
	attacks |= (pawn >> 7) & enemy & ~col_a;
	attacks |= (pawn >> 9) & enemy & ~col_h;
	if (en_passant != -1) {
		uint64_t ep = 1ULL << en_passant;
		if ((pawn >> 7) & ep & ~col_a) attacks |= ep;
		if ((pawn >> 9) & ep & ~col_h) attacks |= ep;
	}
	return attacks;
}

uint64_t get_rook_attacks (int from, uint64_t friendly, uint64_t enemy) {
	uint64_t attacks = 0;
	static const int dirs[4] = {1, -1, 8, -8};
	for (int i = 0; i < 4; i++) {
		int a = from;
		while (1) {
			int next = a + dirs[i];
			if (next < 0 || next > 63) break;
			if (dirs[i] == 1 && (next & 7) == 0) break;
			if (dirs[i] == -1 && (next & 7) == 7) break;
			uint64_t bit = 1ULL << next;
			if (bit & friendly) break;
			attacks |= bit;
			if (bit & enemy) break;
			a = next;
		}
	}
	return attacks;
}

uint64_t get_alfil_attacks (int from, uint64_t friendly, uint64_t enemy) {
	uint64_t attacks = 0;
	static const int dirs[4] = {9, -9, 7, -7};
	for (int i = 0; i < 4; i++) {
		int a = from;
		while (1) {
			int next = a + dirs[i];
			if (next < 0 || next > 63) break;
			if ((dirs[i] == 9 || dirs[i] == -7) && (next & 7) == 0) break;
			if ((dirs[i] == -9 || dirs[i] == 7) && (next & 7) == 7) break;

			uint64_t bit = 1ULL << next;
			if (bit & friendly) break;
			attacks |= bit;
			if (bit & enemy) break;
			a = next;
		}
	}
	return attacks;
}

uint64_t get_queen_attacks (int from, uint64_t friendly, uint64_t enemy) {
	return get_alfil_attacks(from, friendly, enemy) | get_rook_attacks(from, friendly, enemy);
}

int get_short_castle_w (uint64_t occupied, Move *out, Pos *pos) {
	if ((pos->castle & 0b0001) && !(occupied & 0x0000000000000060ULL) && !is_attacked(4, 0, pos) && !is_attacked(5, 0, pos) && !is_attacked(6, 0, pos)) { 
		*out = (Move){4, 6, -1, 5};
		return 1;
	}
	return 0;
}

int get_long_castle_w (uint64_t occupied, Move *out, Pos *pos) {
	if ((pos->castle & 0b0010) && !(occupied & 0x000000000000000EULL) && !is_attacked(4, 0, pos) && !is_attacked(3, 0, pos) && !is_attacked(2, 0, pos)) { 
		*out = (Move){4, 2, -1, 5};
		return 1;
	}
	return 0;
}

int get_short_castle_b (uint64_t occupied, Move *out, Pos *pos) {
	if ((pos->castle & 0b0100) && !(occupied & 0x6000000000000000ULL) && !is_attacked(60, 1, pos) && !is_attacked(61, 1, pos) && !is_attacked(62, 1, pos)) { 
		*out = (Move){60, 62, -1, 11};
		return 1;
	}
	return 0;
}

int get_long_castle_b (uint64_t occupied, Move *out, Pos *pos) {
	if ((pos->castle & 0b1000) && !(occupied & 0x0E00000000000000ULL) && !is_attacked(60, 1, pos) && !is_attacked(59, 1, pos) && !is_attacked(58, 1, pos)) { 
		*out = (Move){60, 58, -1, 11};
		return 1;
	}
	return 0;
}
