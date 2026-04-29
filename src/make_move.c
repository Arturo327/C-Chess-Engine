#include "make_move.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void apply_move (Move *m, Pos *pos) {
	pos->hash ^= zob_castle[pos->castle & 0xF];
	int old_ep_idx = (pos->en_passant == -1) ? 8 : (pos->en_passant & 7);
    	pos->hash ^= zob_ep[old_ep_idx];

	pos->hash ^= zob_pieces[m->pieza][m->from];
    	pos->hash ^= zob_pieces[m->pieza][m->to];
	pos->bitboard[m->pieza] ^= (1ULL << m->from) | (1ULL << m->to);
	pos->board[m->to] = m->pieza;
	pos->board[m->from] = -1;

	if (m->capture != -1) {
		pos->hash ^= zob_pieces[m->capture][m->to];
		pos->bitboard[m->capture] &= ~(1ULL << m->to);
	}

	// en passant capture
	pos->en_passant = -1;
    	if (m->capture == -1 && m->pieza == 0 && (m->to & 7) != (m->from & 7)) {
        	int ep_sq = m->to - 8;
        	pos->hash ^= zob_pieces[6][ep_sq];
        	pos->bitboard[6] &= ~(1ULL << ep_sq);
       	 	pos->board[ep_sq] = -1;
    	}
    	if (m->capture == -1 && m->pieza == 6 && (m->to & 7) != (m->from & 7)) {
        	int ep_sq = m->to + 8;
        	pos->hash ^= zob_pieces[0][ep_sq];
        	pos->bitboard[0] &= ~(1ULL << ep_sq);
        	pos->board[ep_sq] = -1;
    	}

	// enroque
	if (m->pieza == 5) {
		pos->castle &= 0b1100;
		if (m->to - m->from == 2) {
			pos->hash ^= zob_pieces[1][7];
            		pos->hash ^= zob_pieces[1][5];
			pos->bitboard[1] ^= (1ULL << 7) | (1ULL << 5);
			pos->board[5] = 1;
			pos->board[7] = -1;
		} else if (m->from - m->to == 2) {
			pos->hash ^= zob_pieces[1][0];
            		pos->hash ^= zob_pieces[1][3];
			pos->bitboard[1] ^= 1ULL | (1ULL << 3);
			pos->board[3] = 1;
			pos->board[0] = -1;
		}
	}

	if (m->pieza == 11) {
		pos->castle &= 0b0011;
		if (m->to - m->from == 2) {
			pos->hash ^= zob_pieces[7][63];
            		pos->hash ^= zob_pieces[7][61];
			pos->bitboard[7] ^= (1ULL << 63) | (1ULL << 61);
			pos->board[61] = 7;
			pos->board[63] = -1;
		} else if (m->from - m->to == 2) {
			pos->hash ^= zob_pieces[7][56];
            		pos->hash ^= zob_pieces[7][59];
			pos->bitboard[7] ^= (1ULL << 59) | (1ULL << 56);
			pos->board[59] = 7;
			pos->board[56] = -1;
		}
	}

	// quitar derecho a enroque
	if (m->pieza == 1) {
		if (m->from == 0) pos->castle &= ~0b0010;
		if (m->from == 7) pos->castle &= ~0b0001;
	}
	if (m->pieza == 7) {
		if (m->from == 56) pos->castle &= ~0b1000;
		if (m->from == 63) pos->castle &= ~0b0100;
	}
	if (m->capture == 1) {
		if (m->to == 0) pos->castle &= ~0b0010;
		if (m->to == 7) pos->castle &= ~0b0001;
	}
	if (m->capture == 7) {
		if (m->to == 56) pos->castle &= ~0b1000;
		if (m->to == 63) pos->castle &= ~0b0100;
	}

	// nuevo en passant
	if (m->pieza == 0 && m->to - m->from == 16) 
		pos->en_passant = m->from + 8;
	if (m->pieza == 6 && m->from - m->to == 16) 
		pos->en_passant = m->from - 8;

	// coronacion
	if (m->pieza == 0 && m->to >= 56) {
		pos->hash ^= zob_pieces[0][m->to];
        	pos->hash ^= zob_pieces[4][m->to];
		pos->bitboard[0] &= ~(1ULL << m->to);
		pos->bitboard[4] |= (1ULL << m->to);
		pos->board[m->to] = 4;
	}

	if (m->pieza == 6 && m->to <= 7) {
		pos->hash ^= zob_pieces[6][m->to];
        	pos->hash ^= zob_pieces[10][m->to];
		pos->bitboard[6] &= ~(1ULL << m->to);
		pos->bitboard[10] |= (1ULL << m->to);
		pos->board[m->to] = 10;
	}

	pos->hash ^= zob_castle[pos->castle & 0xF];
    	int new_ep_idx = (pos->en_passant == -1) ? 8 : (pos->en_passant & 7);
    	pos->hash ^= zob_ep[new_ep_idx];
    	pos->side ^= 1;
    	pos->hash ^= zob_turn;
}
