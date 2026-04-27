#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "get_captures.h"

int get_w_captures (Move *moves, Pos *pos) {
	uint64_t friendly = pos->bitboard[0] | pos->bitboard[1] | pos->bitboard[2] | pos->bitboard[3] | pos->bitboard[4] | pos->bitboard[5];
	uint64_t enemy = pos->bitboard[6] | pos->bitboard[7] | pos->bitboard[8] | pos->bitboard[9] | pos->bitboard[10] | pos->bitboard[11];
	uint64_t occupied = friendly | enemy;

	uint64_t piezas;
	uint64_t attacks;

	int total_moves = 0;
	Move *actual_move = moves;

	int sq;
	piezas = pos->bitboard[0];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = get_w_pawn_captures(sq, enemy, pos->en_passant);
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

			actual_move->capture = pos->board[actual_move->to];
			actual_move->pieza = 0;

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[2];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = horse_moves[sq] & enemy;
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

			actual_move->pieza = 2;
			actual_move->capture = pos->board[actual_move->to];

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[3];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = get_alfil_attacks(sq, friendly, enemy) & enemy;
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

			actual_move->pieza = 3;
			actual_move->capture = pos->board[actual_move->to];

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[1];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = get_rook_attacks(sq, friendly, enemy) & enemy;
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

			actual_move->pieza = 1;
			actual_move->capture = pos->board[actual_move->to];

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[4];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = get_queen_attacks(sq, friendly, enemy) & enemy;
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

			actual_move->pieza = 4;
			actual_move->capture = pos->board[actual_move->to];

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[5];
	int king_sq = __builtin_ctzll(pos->bitboard[5]);
	attacks = king_moves[king_sq] & enemy;
	while (attacks) {
		actual_move->from = king_sq;
    		actual_move->pieza = 5;
    		actual_move->to = __builtin_ctzll(attacks);
    		attacks &= attacks - 1;
    		actual_move->capture = pos->board[actual_move->to];
    		actual_move++;
    		total_moves++;
	}

	return total_moves;
}

int get_b_captures (Move *moves, Pos *pos) {
	uint64_t friendly = pos->bitboard[6] | pos->bitboard[7] | pos->bitboard[8] | pos->bitboard[9] | pos->bitboard[10] | pos->bitboard[11];
	uint64_t enemy = pos->bitboard[0] | pos->bitboard[1] | pos->bitboard[2] | pos->bitboard[3] | pos->bitboard[4] | pos->bitboard[5];
	uint64_t occupied = friendly | enemy;

	uint64_t piezas;
	uint64_t attacks;

	int total_moves = 0;
	Move *actual_move = moves;
	int sq;

	piezas = pos->bitboard[6];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = get_b_pawn_captures(sq, enemy, pos->en_passant);
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

    			actual_move->pieza = 6;
			actual_move->capture = pos->board[actual_move->to];

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[8];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = horse_moves[sq] & enemy;
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

    			actual_move->pieza = 8;
			actual_move->capture = pos->board[actual_move->to];

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[9];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = get_alfil_attacks(sq, friendly, enemy) & enemy;
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

    			actual_move->pieza = 9;
			actual_move->capture = pos->board[actual_move->to];

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[7];
	while (piezas) {
		sq = __builtin_ctzll(piezas);
		piezas &= piezas - 1;
		attacks = get_rook_attacks(sq, friendly, enemy) & enemy;
		while (attacks) {
			actual_move->from = sq;
			actual_move->to = __builtin_ctzll(attacks);
			attacks &= attacks - 1;

    			actual_move->pieza = 7;
			actual_move->capture = pos->board[actual_move->to];

			actual_move++;
			total_moves++;
		}
	}

	piezas = pos->bitboard[10];
	while (piezas) {
    		sq = __builtin_ctzll(piezas);
    		piezas &= piezas - 1;
    		attacks = get_queen_attacks(sq, friendly, enemy) & enemy;
    		while (attacks) {
        		actual_move->from = sq;
        		actual_move->to = __builtin_ctzll(attacks);
        		attacks &= attacks - 1;
        
			actual_move->pieza = 10;
        		actual_move->capture = pos->board[actual_move->to];

        		actual_move++;
        		total_moves++;
    		}
	}


	piezas = pos->bitboard[11];
	int king_sq = __builtin_ctzll(piezas);
	attacks = king_moves[king_sq] & enemy;
	while (attacks) {
		actual_move->from = king_sq;
    		actual_move->pieza = 11;
    		actual_move->to = __builtin_ctzll(attacks);
    		attacks &= attacks - 1;
    		actual_move->capture = pos->board[actual_move->to];
    		actual_move++;
    		total_moves++;
	}

	return total_moves;
}
