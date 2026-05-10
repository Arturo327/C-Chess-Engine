#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "movegen.h"
#include "make_move.h"
#include "bitboards.h"

int is_attacked(int sq, int by_white, Pos *pos) {
	uint64_t occupied = 0;
	for (int i = 0; i < 12; i++) occupied |= pos->bitboard[i];

	int pawn = by_white ? 0 : 6;
	uint64_t pawns = pos->bitboard[pawn];
	if (pawns) {
		uint64_t patt;
        	if (by_white) {
            		uint64_t col_a = 0x0101010101010101ULL;
            		uint64_t col_h = 0x8080808080808080ULL;
            		uint64_t target = 1ULL << sq;
            		patt = ((target >> 7) & ~col_a) | ((target >> 9) & ~col_h);
        	} else {
            		uint64_t col_a = 0x0101010101010101ULL;
            		uint64_t col_h = 0x8080808080808080ULL;
            		uint64_t target = 1ULL << sq;
            		patt = ((target << 7) & ~col_h) | ((target << 9) & ~col_a);
        	}
        	patt &= pawns;
        	if (patt) return 1;
	}

	int horse = by_white ? 2 : 8;
    	uint64_t horses = pos->bitboard[horse] & horse_moves[sq];
    	if (horses) return 1;

	uint64_t diag_attackers = pos->bitboard[by_white ? 3 : 9] | pos->bitboard[by_white ? 4 : 10];	
	if (diag_attackers) {
		uint64_t occ = occupied & alfil_masks[sq];
		int idx = (occ * alfil_magics[sq]) >> (64 - alfil_bits[sq]);
		uint64_t diag_att = alfil_attack_table[sq][idx] & diag_attackers;
		if (diag_att) return 1;
	}

	uint64_t line_attackers = pos->bitboard[by_white ? 1 : 7] | pos->bitboard[by_white ? 4 : 10];
	if (line_attackers) {
		uint64_t occ = occupied & rook_masks[sq];
		int idx = (occ * rook_magics[sq]) >> (64 - rook_bits[sq]);
		uint64_t line_att = rook_attack_table[sq][idx] & line_attackers;
		if (line_att) return 1;
	}

	int king = by_white ? 5 : 11;
    	uint64_t kings = pos->bitboard[king] & king_moves[sq];
    	if (kings) return 1;

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
	uint64_t occ = (friendly | enemy) & rook_masks[from];
	int idx = (occ * rook_magics[from]) >> (64 - rook_bits[from]);
	return rook_attack_table[from][idx] & ~friendly;
}

uint64_t get_alfil_attacks (int from, uint64_t friendly, uint64_t enemy) {
	uint64_t occ = (friendly | enemy) & alfil_masks[from];
	int idx = (occ * alfil_magics[from]) >> (64 - alfil_bits[from]);
	return alfil_attack_table[from][idx] & ~friendly;
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
