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

int quiescence (Pos *pos, int depth, int alpha, int beta) {
	int side = pos->side;
	int stand_pat = (side == 0) ? eval_pos(pos) : -eval_pos(pos);
	if (depth == 0) return stand_pat;

	if (stand_pat + 1000 < alpha) return alpha;
	if (stand_pat >= beta) return beta;
	if (stand_pat > alpha) alpha = stand_pat;

	Move captures[64];
	Move *actual_move = captures;
	int count;
	if (side) count = get_b_captures(captures, pos);
	else count = get_w_captures(captures, pos);
	sort_moves(captures, count, 0);

	for (int i = 0; i < count; i++) {
		Pos child = *pos;
		apply_move(actual_move, &child);

		int king_sq = __builtin_ctzll(child.bitboard[side ? 11 : 5]);
		if (is_attacked(king_sq, side, &child)) { actual_move++; continue; }

		int eval = -quiescence(&child, depth - 1, -beta, -alpha);

		if (eval > alpha) alpha = eval;
		if (alpha >= beta) return beta;
		actual_move++;
	}
	return alpha;
}

int negamax (Pos *pos, int depth, int ply, int alpha, int beta, Move *best_out) {
	if (depth == 0) return quiescence(pos, 5, alpha, beta);
	int side = pos->side;

	int original_alpha = alpha;

	TTEntry *entry = tt_probe(pos->hash);
	int score = 0;
	if (entry) {
    		score = entry->score;
    		if (score > 99000) score -= ply;
    		else if (score < -99000) score += ply;
    		if (entry->depth >= depth) {
        		if (best_out && entry->best.from != entry->best.to)
            			*best_out = entry->best;
        		if (entry->flag == TT_EXACT) return score;
        		if (entry->flag == TT_LOWER && score >= beta) return score;
        		if (entry->flag == TT_UPPER && score <= alpha) return score;
    		}
	}

	if (depth >= 3 && !is_attacked(__builtin_ctzll(pos->bitboard[side ? 11 : 5]), side, pos)) {
    		Pos null_pos = *pos;
    		null_pos.side ^= 1;
    		null_pos.hash ^= zob_turn;
		int old_ep_idx = (null_pos.en_passant == -1) ? 8 : (null_pos.en_passant & 7);
		null_pos.en_passant = -1;
		null_pos.hash ^= zob_ep[old_ep_idx];

    		int reduction = 3;
		int c = depth - 1 - reduction;
		if (c < 0) c = 0;
    		int null_eval = -negamax(&null_pos, c, ply + 1, -beta, -beta + 1, NULL);
    		if (null_eval >= beta) return beta;
	}

	Move moves[256];
	int total_moves;
	if (side) total_moves = get_b_moves(moves, pos);
	else total_moves = get_w_moves(moves, pos);
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

	for (int i = 0; i < total_moves; i++) {
		child = *pos;
		apply_move(actual_move, &child);

		int king_sq = __builtin_ctzll(child.bitboard[side ? 11 : 5]);
		if (is_attacked(king_sq, side, &child)) {
			actual_move++;
			continue;
		}
		
		int eval;
		if (i == 0) {
			eval = -negamax(&child, depth - 1, ply + 1, -beta, -alpha, NULL);
		} else {
			int reduction = 0;
    			if (i >= 3 && depth >= 3 && actual_move->capture == -1) {
        			reduction = 1;
        			if (i >= 6) reduction = depth / 3;
    			}
			eval = -negamax(&child, depth - 1 - reduction, ply + 1, -alpha - 1, -alpha, NULL);
			if (eval > alpha && eval < beta) {
				eval = -negamax(&child, depth - 1, ply + 1, -beta, -alpha, NULL);
			}
		}

		if (eval > maxEval) {
			maxEval = eval;
			best_move = *actual_move;
			if (best_out) *best_out = *actual_move;
		}

		if (eval > alpha)
			alpha = eval;

		if (alpha >= beta) {
			store_killer(actual_move, ply);
			update_history(actual_move, depth);
			break;
		}
		actual_move++;
	}

	if (maxEval == -1000000) {
		int king_sq = __builtin_ctzll(pos->bitboard[side ? 11 : 5]);
		int score = is_attacked(king_sq, side, pos) ? -(100000 - ply) : 0;
		int a = score;
		if (score < -99000) a = score - ply;
		tt_store(pos->hash, a, depth, TT_EXACT, NULL);
		return score;
	}

	int flag;
	if (maxEval <= original_alpha) flag = TT_UPPER;
	else if (maxEval >= beta) flag = TT_LOWER;
	else flag = TT_EXACT;
	tt_store(pos->hash, maxEval, depth, flag, &best_move);

	return maxEval;
}

Move bot_move (int depth, Pos *pos) {
	Move best_move = {0};
	shift_killers();
	int prev_score = 0;
	for (int i = 1; i <= depth; i++) {
		int delta = 50;
		int alpha = prev_score - delta;
		int beta = prev_score + delta;
		int score;
		while (1) {
			score = negamax(pos, i, 0, -1000000, 1000000, &best_move);
			if (score <= alpha) alpha -= delta * 2;
            		else if (score >= beta) beta += delta * 2;
            		else break;
            		delta *= 2;
		}
		prev_score = score;
	}
	return best_move;
}









