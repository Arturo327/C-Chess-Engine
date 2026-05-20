#include "bitboards.h"
#include <stdlib.h>
#include <string.h>

uint64_t rook_attack_table[64][4096] = {0};
uint64_t alfil_attack_table[64][512] = {0};

uint64_t rook_magics[64] = {0};
uint64_t alfil_magics[64] = {0};

uint64_t rook_masks[64] = {0};
uint64_t alfil_masks[64] = {0};

int rook_bits[64] = {0};
int alfil_bits[64] = {0};

uint64_t pawn_attacks[2][64] = {0};

static uint64_t state = 0x123456789ABCDEF0ULL;
static uint64_t xorshift64(void) {
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return state;
}

static uint64_t rook_mask (int sq) {
	uint64_t mask = 0;
	int fila = sq >> 3;
	int col = sq % 8;
	for (int i = 1; i < 7; i++) {
		if (i != col) mask |= 1ULL << ((fila << 3) + i);
		if (i != fila) mask |= 1ULL << ((i << 3) + col);
	}
	return mask;
}

static uint64_t alfil_mask (int sq) {
	uint64_t mask = 0;
	int fila = sq >> 3;
	int col = sq % 8;
	for (int i = fila + 1, j = col + 1; i < 7 && j < 7; i++, j++)
		mask |= 1ULL << ((i << 3) + j);
	for (int i = fila + 1, j = col - 1; i < 7 && j > 0; i++, j--)
		mask |= 1ULL << ((i << 3) + j);
	for (int i = fila - 1, j = col + 1; i > 0 && j < 7; i--, j++)
		mask |= 1ULL << ((i << 3) + j);
	for (int i = fila - 1, j = col - 1; i > 0 && j > 0; i--, j--)
		mask |= 1ULL << ((i << 3) + j);
	return mask;
}

static uint64_t rook_attacks_slow (int sq, uint64_t occ) {
	uint64_t attacks = 0;
	int fila = sq >> 3;
	int col = sq % 8;
	for (int i = fila + 1; i <= 7; i++) {
		uint64_t a = 1ULL << ((i << 3) + col);
		attacks |= a;
		if (a & occ) break;
	}
	for (int i = fila - 1; i >= 0; i--) {
		uint64_t a = 1ULL << ((i << 3) + col);
		attacks |= a;
		if (a & occ) break;
	}
	for (int i = col + 1; i <= 7; i++) {
		uint64_t a = 1ULL << ((fila << 3) + i);
		attacks |= a;
		if (a & occ) break;
	}
	for (int i = col - 1; i >= 0; i--) {
		uint64_t a = 1ULL << ((fila << 3) + i);
		attacks |= a;
		if (a & occ) break;
	}
	return attacks;
}

static uint64_t alfil_attacks_slow (int sq, uint64_t occ) {
	uint64_t attacks = 0;
	int fila = sq >> 3;
	int col = sq % 8;
	for (int i = fila + 1, j = col + 1; i <= 7 && j <= 7; i++, j++) {
		uint64_t a = 1ULL << ((i << 3) + j);
		attacks |= a;
		if (a & occ) break;
	}
	for (int i = fila + 1, j = col - 1; i <= 7 && j >= 0; i++, j--) {
		uint64_t a = 1ULL << ((i << 3) + j);
		attacks |= a;
		if (a & occ) break;
	}
	for (int i = fila - 1, j = col + 1; i >= 0 && j <= 7; i--, j++) {
		uint64_t a = 1ULL << ((i << 3) + j);
		attacks |= a;
		if (a & occ) break;
	}

	for (int i = fila - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
		uint64_t a = 1ULL << ((i << 3) + j);
		attacks |= a;
		if (a & occ) break;
	}
	return attacks;
}

static uint64_t find_magic(int sq, int bits, int is_rook) {
	uint64_t mask = is_rook ? rook_masks[sq] : alfil_masks[sq];
	int n = __builtin_popcountll(mask);
	int size = 1 << n;

	uint64_t occs[4096];
	uint64_t atts[4096];
	uint64_t occ = 0;
	for (int i = 0; i < size; i++) {
		occs[i] = occ;
		atts[i] = is_rook ? rook_attacks_slow(sq, occ) : alfil_attacks_slow(sq, occ);
		occ = (occ - mask) & mask;
	}

	uint64_t used[4096];
	while (1) {
		uint64_t magic = xorshift64() & xorshift64() & xorshift64();
		if (__builtin_popcountll((mask * magic) >> 56) < 6) continue;

		memset(used, 0, size * sizeof(uint64_t));
		int ok = 1;
		for (int i = 0; i < size; i++) {
			int idx = (occs[i] * magic) >> (64 - bits);
			if (used[idx] == 0) used[idx] = atts[i];
			else if (used[idx] != atts[i]) { ok = 0; break; }
		}
		if (ok) return magic;
	}
}

void init_magics(void) {
	for (int sq = 0; sq < 64; sq++) {
		rook_masks[sq] = rook_mask(sq);
		alfil_masks[sq] = alfil_mask(sq);
		rook_bits[sq] = __builtin_popcountll(rook_masks[sq]);
		alfil_bits[sq] = __builtin_popcountll(alfil_masks[sq]);

		rook_magics[sq] = find_magic(sq, rook_bits[sq],	1);
		alfil_magics[sq] = find_magic(sq, alfil_bits[sq], 0);

		int n = rook_bits[sq], size = 1 << n;
		uint64_t occ = 0;
		for (int i = 0; i < size; i++) {
			int idx = (occ * rook_magics[sq]) >> (64 - n);
			rook_attack_table[sq][idx] = rook_attacks_slow(sq, occ);
			occ = (occ - rook_masks[sq]) & rook_masks[sq];
		}
		n = alfil_bits[sq]; 
		size = 1 << n;
		occ = 0;
		for (int i = 0; i < size; i++) {
			int idx = (occ * alfil_magics[sq]) >> (64 - n);
			alfil_attack_table[sq][idx] = alfil_attacks_slow(sq, occ);
			occ = (occ - alfil_masks[sq]) & alfil_masks[sq];
		}

		pawn_attacks[0][sq] = 0;
		pawn_attacks[1][sq] = 0;

		if ((sq & 7) != 0) {
			if (sq <= 56) pawn_attacks[0][sq] |= 1ULL << (sq + 7);
			if (sq >= 9) pawn_attacks[1][sq] |= 1ULL << (sq - 9);
		}

		if ((sq & 7) != 7) {
			if (sq <= 54) pawn_attacks[0][sq] |= 1ULL << (sq + 9);
			if (sq >= 7) pawn_attacks[1][sq] |= 1ULL << (sq - 7);
		}
	}
}












