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

	int original_alpha = alpha;
	TTEntry *entry = tt_probe(pos->hash);
	if (entry && entry->depth >= depth) {
		if (entry->flag == TT_EXACT) return entry->score;
		if (entry->flag == TT_LOWER && entry->score >= beta)  return entry->score;
		if (entry->flag == TT_UPPER && entry->score <= alpha) return entry->score;
	}
	Move moves[256];
	int total_moves = get_w_moves(moves, pos);
	Move *actual_move = moves;

	Move tt_best_move = {0};
	int has_tt_move = (entry != NULL);

	if (has_tt_move) {
		tt_best_move = entry->best;
		for (int i = 0; i < total_moves; i++) {
			if (actual_move->from == tt_best_move.from && actual_move->to == tt_best_move.to) {
				Move tmp = moves[0];
				moves[0] = *actual_move;
				*actual_move = tmp;
				break;
			}
			actual_move++;
		}
	}

	sort_moves(moves + has_tt_move, total_moves - has_tt_move);

	int maxEval = -1000000;
	Pos child;
	Move best_move = moves[0];
	actual_move = moves;

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
			best_move = *actual_move;	
			if (best_out)*best_out = *actual_move; 
		}
		if (eval > alpha) alpha = eval; 
		if (beta <= alpha) {
			tt_store(pos->hash, maxEval, depth, TT_LOWER, &best_move);
			return maxEval; 
		}

		actual_move++;
	}

	int flag = (maxEval <= original_alpha) ? TT_UPPER : TT_EXACT;
	tt_store(pos->hash, maxEval, depth, flag, &best_move);

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

	int original_beta = beta;
	TTEntry *entry = tt_probe(pos->hash);
	if (entry && entry->depth >= depth) {
		if (entry->flag == TT_EXACT) return entry->score;
		if (entry->flag == TT_LOWER && entry->score >= beta) return entry->score;
		if (entry->flag == TT_UPPER && entry->score <= alpha) return entry->score;
	}
	Move moves[256];
	int total_moves = get_b_moves(moves, pos);
	Move *actual_move = moves;

	Move tt_best_move = {0};
	int has_tt_move = (entry != NULL);

	if (has_tt_move) {
		tt_best_move = entry->best;
		for (int i = 0; i < total_moves; i++) {
			if (actual_move->from == tt_best_move.from && actual_move->to == tt_best_move.to) {
				Move tmp = moves[0];
				moves[0] = *actual_move;
				*actual_move = tmp;
				break;
			}
			actual_move++;
		}
	}

	sort_moves(moves + has_tt_move, total_moves - has_tt_move);

	int minEval = 1000000;
	actual_move = moves;
	Move best_move = moves[0];
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
			best_move = *actual_move;	
			if (best_out) *best_out = *actual_move; 
		}
		if (eval < beta) beta = eval; 
		if (beta <= alpha) {
			tt_store(pos->hash, minEval, depth, TT_UPPER, &best_move);
			return minEval; 
		}

		actual_move++;
	}

	int flag = (minEval >= original_beta) ? TT_LOWER : TT_EXACT;
	tt_store(pos->hash, minEval, depth, flag, &best_move);
	return minEval;
}

Move bot_move (int depth, int withe, Pos *pos) {
	Move moves[256];
	Move best_move = moves[0];
	if (withe) search_moves_withe(pos, depth, -1000000, 1000000, &best_move);
	else search_moves_black(pos, depth, -1000000, 1000000, &best_move);
	return best_move;
}




















