#include "motor.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int quiescence_w(Pos *pos, int depth, int alpha, int beta) {
	int stand_pat = eval_pos(pos);
	if (depth == 0) return stand_pat;
	if (stand_pat >= beta) return beta;
	if (stand_pat > alpha) alpha = stand_pat;

	Move captures[64];
	Move *actual_move = captures;
	int count = get_w_captures(captures, pos);
	sort_moves(captures, count);
	Pos child;
	for (int i = 0; i < count; i++) {
		child = *pos;
		apply_move(actual_move, &child);
		int eval = quiescence_b(&child, depth - 1, alpha, beta);

		if (eval > alpha) alpha = eval;
		if (alpha >= beta) return beta;
		actual_move++;
	}
	return alpha;
}

int search_moves_withe (Pos *pos, int depth, int alpha, int beta, Move *best_out)  {
	if (depth == 0) return quiescence_w (pos, 5, alpha, beta);
	Move moves[256];
	int total_moves = get_w_moves(moves, pos);
	sort_moves(moves, total_moves);

	int maxEval = -1000000;
	Move *actual_move = moves;
	Pos child;

	for (int i = 0; i < total_moves; i++) {
		child = *pos;
		apply_move(actual_move, &child);
	       	int king_sq = __builtin_ctzll(child.bitboard[5]);
        	if (is_attacked(king_sq, 0, &child)) {
            		actual_move++;
            		continue;
        	}
		int eval = search_moves_black(&child, depth - 1, alpha, beta, NULL); 

		if (eval > maxEval) { 
			maxEval = eval; 
			if (best_out) *best_out = *actual_move; 
		}
		if (eval > alpha) alpha = eval; 
		if (beta <= alpha) return maxEval; 

		actual_move++;
	}

	return maxEval;
}

int quiescence_b(Pos *pos, int depth, int alpha, int beta) {
	int stand_pat = eval_pos(pos);
	if (depth == 0) return stand_pat;
	if (stand_pat <= alpha) return alpha;
	if (stand_pat < beta) beta = stand_pat;

	Move captures[64];
	int count = get_b_captures(captures, pos);
	sort_moves(captures, count);
	Pos child;
	Move *actual_move = captures;

	for (int i = 0; i < count; i++) {
		child = *pos;
		apply_move(actual_move, &child);
		int eval = quiescence_w(&child, depth - 1, alpha, beta);

		if (eval < beta) beta = eval;
		if (alpha >= beta) return alpha;
		actual_move++;
	}
	return beta;
}

int search_moves_black (Pos *pos, int depth, int alpha, int beta, Move *best_out)  {
	if (depth == 0) return quiescence_b (pos, 5, alpha, beta);
	Move moves[256];
	int total_moves = get_b_moves(moves, pos);
	sort_moves(moves, total_moves);

	int minEval = 1000000;
	Move *actual_move = moves;
	Pos child;

	for (int i = 0; i < total_moves; i++) {
		child = *pos;
		apply_move(actual_move, &child);
	       	int king_sq = __builtin_ctzll(child.bitboard[11]);
        	if (is_attacked(king_sq, 1, &child)) {
            		actual_move++;
            		continue;
        	}
		int eval = search_moves_withe(&child, depth - 1, alpha, beta, NULL);

		if (eval < minEval) { 
			minEval = eval; 
			if (best_out) *best_out = *actual_move; 
		}
		if (eval < beta) beta = eval; 
		if (beta <= alpha) return minEval; 

		actual_move++;
	}

	return minEval;
}

Move bot_move (int depth, int withe, Pos *pos) {
	Move moves[256];
	Move best_move = moves[0];
	if (withe) search_moves_withe(pos, depth, -1000000, 1000000, &best_move);
	else search_moves_black(pos, depth, -1000000, 1000000, &best_move);
	return best_move;
}




















