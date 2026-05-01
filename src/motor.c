#include "motor.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static void update_history(Move *m, int depth) {
	if (m->capture != -1) return;
	history[m->pieza][m->to] += depth * depth;

	if (history[m->pieza][m->to] > 1000000) {
		for (int p = 0; p < 12; p++)
			for (int sq = 0; sq < 64; sq++)
				history[p][sq] /= 2;
	}
}

static void store_killer(Move *m, int ply) {
	if (m->capture != -1) return;

	if (killers[ply][0].from == m->from && killers[ply][0].to == m->to) return;

	killers[ply][1] = killers[ply][0];
	killers[ply][0] = *m;
}

void shift_killers(void) {
	for (int i = 0; i < MAX_DEPTH - 2; i++) {
		killers[i][0] = killers[i + 2][0];
		killers[i][1] = killers[i + 2][1];
	}
	memset(&killers[MAX_DEPTH - 2], 0, 2 * sizeof(killers[0]));
}

int quiescence_w(Pos *pos, int depth, int alpha, int beta) {
	int stand_pat = eval_pos(pos);
	if (depth == 0) return stand_pat;
	if (stand_pat >= beta) return beta;
	if (stand_pat > alpha) alpha = stand_pat;

	Move captures[64];
	Move *actual_move = captures;
	int count = get_w_captures(captures, pos);
	sort_moves(captures, count, 0);
	Pos child;
	for (int i = 0; i < count; i++) {
		child = *pos;
		apply_move(actual_move, &child);
		int king_sq = __builtin_ctzll(child.bitboard[5]);
		if (is_attacked(king_sq, 0, &child)) { actual_move++; continue; }
		int eval = quiescence_b(&child, depth - 1, alpha, beta);

		if (eval > alpha) alpha = eval;
		if (alpha >= beta) return beta;
		actual_move++;
	}
	return alpha;
}

int search_moves_withe (Pos *pos, int depth, int ply, int alpha, int beta, Move *best_out)  {
	if (depth == 0) return quiescence_w (pos, 5, alpha, beta);

	int original_alpha = alpha;
	TTEntry *entry = tt_probe(pos->hash);
	if (entry && entry->depth >= depth) {
		if (best_out && entry->best.from != entry->best.to)
			*best_out = entry->best;
		if (entry->flag == TT_EXACT) return entry->score;
		if (entry->flag == TT_LOWER && entry->score >= beta)  return entry->score;
		if (entry->flag == TT_UPPER && entry->score <= alpha) return entry->score;
	}
	Move moves[256];
	int total_moves = get_w_moves(moves, pos);
	Move *actual_move = moves;

	Move tt_best_move = {0};
	int has_tt_move = (entry != NULL && entry->best.from != entry->best.to);

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

	sort_moves(moves + has_tt_move, total_moves - has_tt_move, ply);

	int maxEval = -1000000;
	Pos child;
	Move best_move = {0};
	actual_move = moves;
	int found_legal = 0;

	for (int i = 0; i < total_moves; i++) {
		child = *pos;
		apply_move(actual_move, &child);
		int king_sq = __builtin_ctzll(child.bitboard[5]);
		if (is_attacked(king_sq, 0, &child)) {
			actual_move++;
			continue;
		}
		if (!found_legal) { 
			best_move = *actual_move; 
			found_legal = 1; 
		}
		int eval = search_moves_black(&child, depth - 1, ply + 1, alpha, beta, NULL); 

		if (eval > maxEval) { 
			maxEval = eval;
			best_move = *actual_move;	
			if (best_out)*best_out = *actual_move; 
		}
		if (eval > alpha) alpha = eval; 
		if (beta <= alpha) {
			store_killer(actual_move, ply);
			update_history(actual_move, depth);
			tt_store(pos->hash, maxEval, depth, TT_LOWER, &best_move);
			return maxEval; 
		}

		actual_move++;
	}

	if (maxEval == -1000000) {
		int king_sq = __builtin_ctzll(pos->bitboard[5]);
    		int score = is_attacked(king_sq, 0, pos) ? -(100000 - ply) : 0;
    		tt_store(pos->hash, score, depth, TT_EXACT, NULL);
    		return score;
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
	sort_moves(captures, count, 0);
	Pos child;
	Move *actual_move = captures;

	for (int i = 0; i < count; i++) {
		child = *pos;
		apply_move(actual_move, &child);
		int king_sq = __builtin_ctzll(child.bitboard[11]);
		if (is_attacked(king_sq, 1, &child)) { actual_move++; continue; }
		int eval = quiescence_w(&child, depth - 1, alpha, beta);

		if (eval < beta) beta = eval;
		if (alpha >= beta) return alpha;
		actual_move++;
	}
	return beta;
}

int search_moves_black (Pos *pos, int depth, int ply, int alpha, int beta, Move *best_out)  {
	if (depth == 0) return quiescence_b (pos, 5, alpha, beta);

	int original_beta = beta;
	TTEntry *entry = tt_probe(pos->hash);
	if (entry && entry->depth >= depth) {
		if (best_out && entry->best.from != entry->best.to)
			*best_out = entry->best;
		if (entry->flag == TT_EXACT) return entry->score;
		if (entry->flag == TT_LOWER && entry->score >= beta) return entry->score;
		if (entry->flag == TT_UPPER && entry->score <= alpha) return entry->score;
	}
	Move moves[256];
	int total_moves = get_b_moves(moves, pos);
	Move *actual_move = moves;

	Move tt_best_move = {0};
	int has_tt_move = (entry != NULL && entry->best.from != entry->best.to);

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

	sort_moves(moves + has_tt_move, total_moves - has_tt_move, ply);

	int minEval = 1000000;
	actual_move = moves;
	Move best_move = {0};
	Pos child;
	int found_legal = 0;

	for (int i = 0; i < total_moves; i++) {
		child = *pos;
		apply_move(actual_move, &child);
		int king_sq = __builtin_ctzll(child.bitboard[11]);
		if (is_attacked(king_sq, 1, &child)) {
			actual_move++;
			continue;
		}
		if (!found_legal) { 
			best_move = *actual_move; 
			found_legal = 1; 
		}
		int eval = search_moves_withe(&child, depth - 1, ply + 1, alpha, beta, NULL);

		if (eval < minEval) { 
			minEval = eval; 
			best_move = *actual_move;	
			if (best_out) *best_out = *actual_move; 
		}
		if (eval < beta) beta = eval; 
		if (beta <= alpha) {
			store_killer(actual_move, ply);
			update_history(actual_move, depth);
			tt_store(pos->hash, minEval, depth, TT_UPPER, &best_move);
			return minEval; 
		}

		actual_move++;
	}

	if (minEval == 1000000) {
		int king_sq = __builtin_ctzll(pos->bitboard[11]);
    		int score = is_attacked(king_sq, 1, pos) ? (100000 - ply) : 0;
    		tt_store(pos->hash, score, depth, TT_EXACT, NULL);
    		return score;
	}

	int flag = (minEval >= original_beta) ? TT_LOWER : TT_EXACT;
	tt_store(pos->hash, minEval, depth, flag, &best_move);
	return minEval;
}

Move bot_move (int depth, int withe, Pos *pos) {
	Move best_move = {0};
	shift_killers();
	for (int i = 1; i <= depth; i++) {
		if (withe) search_moves_withe(pos, i, 0, -1000000, 1000000, &best_move);
		else search_moves_black(pos, i, 0, -1000000, 1000000, &best_move);
	}
	return best_move;
}









