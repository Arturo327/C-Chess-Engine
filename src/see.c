#include "see.h"
#include "movegen.h"
#include "bitboards.h"

static int least_valuable_attacker (int sq, int side, uint64_t occupied, Pos *pos, uint64_t *used) {
	int pawn = side ? 6 : 0;
	uint64_t pawns = pos->bitboard[pawn] & ~(*used);
	if (pawns) {
		uint64_t patt;
        	if (side == 0) {
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
        	if (patt) {
            		*used |= patt & -patt;
            		return values[0];
        	}
	}

	int horse = side ? 8 : 2;
    	uint64_t horses = pos->bitboard[horse] & horse_moves[sq] & ~(*used);
    	if (horses) {
        	*used |= horses & -horses;
        	return values[2];
    	}

	uint64_t diag_attackers = (pos->bitboard[side ? 9 : 3] | pos->bitboard[side ? 10 : 4]) & ~(*used);	
	if (diag_attackers) {
		uint64_t occ = occupied & alfil_masks[sq];
		int idx = (occ * alfil_magics[sq]) >> (64 - alfil_bits[sq]);
		uint64_t diag_att = alfil_attack_table[sq][idx] & diag_attackers;
		if (diag_att) {
            		int piece = (pos->bitboard[side ? 9 : 3] & diag_att) ? 3 : 4;
            		*used |= diag_att & -diag_att;
            		return values[piece == 3 ? 3 : 4];
        	}
	}

	uint64_t line_attackers = (pos->bitboard[side ? 7 : 1] | pos->bitboard[side ? 10 : 4]) & ~(*used);
	if (line_attackers) {
		uint64_t occ = occupied & rook_masks[sq];
		int idx = (occ * rook_magics[sq]) >> (64 - rook_bits[sq]);
		uint64_t line_att = rook_attack_table[sq][idx] & line_attackers;
		if (line_att) {
            		int piece = (pos->bitboard[side ? 7 : 1] & line_att) ? 1 : 4;
            		*used |= line_att & -line_att;
            		return values[piece == 1 ? 1 : 4];
        	}
	}

	int king = side ? 11 : 5;
    	uint64_t kings = pos->bitboard[king] & king_moves[sq] & ~(*used);
    	if (kings) {
        	*used |= kings & -kings;
        	return values[5];
    	}

    	return -1;
}

int see(Pos *pos, int to, int target_val, int from, int piece_val) {
	uint64_t used = 0;
	used |= (1ULL << from);
	uint64_t occupied = 0;
	for (int i = 0; i < 12; i++) occupied |= pos->bitboard[i];
	int gain[32];
	int d = 0;
	gain[d] = target_val;
	int side = pos->side ^ 1;
	int attacker_val = piece_val;

	while (1) {
		d++;
		gain[d] = attacker_val - gain[d - 1];
		if (gain[d] < 0) break;
		occupied &= ~used;
		attacker_val = least_valuable_attacker(to, side, occupied, pos, &used);
		if (attacker_val == -1) break;
		side ^= 1;
	}

	while (--d > 0)
		gain[d - 1] = -gain[d] > gain[d - 1] ? -gain[d] : gain[d - 1];

	return gain[0];
}
