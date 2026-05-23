#include "zobrist.h"
#include <stdlib.h>

uint64_t zob_pieces[12][64];
uint64_t zob_turn;
uint64_t zob_castle[16];
uint64_t zob_ep[9];

static uint64_t state = 0x123456789ABCDEF0ULL;
static uint64_t xorshift64 (void) {
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return state;
}

void init_zobrist (void) {
	for (int p = 0; p < 12; p++)
		for (int sq = 0; sq < 64; sq++)
			zob_pieces[p][sq] = xorshift64();

	zob_turn = xorshift64();
	for (int i = 0; i < 16; i++)
		zob_castle[i] = xorshift64();

	for (int i = 0; i < 9; i++)
		zob_ep[i] = xorshift64();
	zob_ep[8] = 0;
}

uint64_t compute_hash (Pos *pos) {
	uint64_t h = 0;
	for (int p = 0; p < 12; p++) {
		uint64_t bb = pos->bitboard[p];
		while (bb) {
			int sq = __builtin_ctzll(bb);
			bb &= bb - 1;
			h ^= zob_pieces[p][sq];
		}
	}
	if (pos->side == 1) h ^= zob_turn;
	h ^= zob_castle[pos->castle & 0xF];
	int ep_idx = (pos->en_passant == -1) ? 8 : (pos->en_passant & 7);
	h ^= zob_ep[ep_idx];

	return h;
}
