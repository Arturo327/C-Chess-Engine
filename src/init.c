#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "init.h"

uint64_t horse_moves[64] = {0};
uint64_t king_moves[64] = {0};

void generate_king_table(void) {
	uint64_t fila_a = 0x0101010101010101ULL;
	uint64_t fila_h = 0x8080808080808080ULL;
	for (int i = 0; i < 64; i++) {
		uint64_t a = 1ULL << i;
		if (!(a & fila_h)) {
			king_moves[i] |= a << 1;
			king_moves[i] |= a << 9;
			king_moves[i] |= a >> 7;
		}
		if (!(a & fila_a)) {
			king_moves[i] |= a >> 1;
			king_moves[i] |= a >> 9;
			king_moves[i] |= a << 7;
		}
		king_moves[i] |= a >> 8;
		king_moves[i] |= a << 8;
	}
}

void generate_horse_table(void) {
	uint64_t fila_a = 0x0101010101010101ULL;
	uint64_t fila_h = 0x8080808080808080ULL;
	uint64_t fila_ab = 0x0303030303030303ULL;  // columnas A y B
	uint64_t fila_hg = 0xC0C0C0C0C0C0C0C0ULL;  // columnas H y G
	for (int i = 0; i < 64; i++) {
		uint64_t a = 1ULL << i;
		if (!(a & fila_h)) {
			horse_moves[i] |= a << 17;
			horse_moves[i] |= a >> 15;
		}
		if (!(a & fila_a)) {
			horse_moves[i] |= a << 15;
			horse_moves[i] |= a >> 17;
		}
		if (!(a & fila_hg)) {
			horse_moves[i] |= a << 10;
			horse_moves[i] |= a >> 6;
		}
		if (!(a & fila_ab)) {
			horse_moves[i] |= a << 6;
			horse_moves[i] |= a >> 10;
		}
	}
}

void reset(Pos *pos) {
	pos->bitboard[0] = 0x000000000000FF00ULL;    //peon blanco
	pos->bitboard[1] = 0x0000000000000081ULL;    //torre blanca
	pos->bitboard[2] = 0x0000000000000042ULL;    //caballo blanco
	pos->bitboard[3] = 0x0000000000000024ULL;    //alfil blanco
	pos->bitboard[4] = 0x0000000000000008ULL;    //reina blanca
	pos->bitboard[5] = 0x0000000000000010ULL;    //rey blanco
	pos->bitboard[6] = 0x00FF000000000000ULL;    //peon negro
	pos->bitboard[7] = 0x8100000000000000ULL;    //torre negra
	pos->bitboard[8] = 0x4200000000000000ULL;    //caballo negro
	pos->bitboard[9] = 0x2400000000000000ULL;    //alfil negro
	pos->bitboard[10] = 0x0800000000000000ULL;   //reina negra
	pos->bitboard[11] = 0x1000000000000000ULL;   //rey negro
	
	pos->en_passant = -1;
	pos->castle = 15; 
	for (int i = 0; i < 64; i++)
		pos->board[i] = -1;
	for (int p = 0; p < 12; p++) {
		uint64_t bb = pos->bitboard[p];
		while (bb) {
			int sq = __builtin_ctzll(bb);
			pos->board[sq] = p;
			bb &= bb - 1;
		}
	}
	pos->side = 0;
	pos->hash = compute_hash(pos);
	pos->occupied = 0xFFFF00000000FFFFULL;
}
