#include "motor.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

uint64_t rep_stack[REP_STACK_SIZE];
int rep_top = 0;

static volatile int interrupted = 0;
static long long nodes = 0;
static long long deadline = 0;

static int lmr_table[64][256] = {0};

void reset_lmr (void) {
	for (int i = 1; i < 64; i++)
		for (int j = 1; j < 256; j++)
			lmr_table[i][j] = (int)(0.75 + log(i) * log(j) / 2.25);
}

static long long get_time_ms (void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

static void update_history (Move *m, int depth) {
	if (m->capture != -1) return;
	history[m->pieza][m->to] += depth * depth;

	if (history[m->pieza][m->to] > 1000000) {
		for (int p = 0; p < 12; p++)
			for (int sq = 0; sq < 64; sq++)
				history[p][sq] >>= 1;
	}
}

static void store_killer (Move *m, int ply) {
	if (m->capture != -1) return;

	if (killers[ply][0].from == m->from && killers[ply][0].to == m->to) return;

	killers[ply][1] = killers[ply][0];
	killers[ply][0] = *m;
}

void shift_killers (void) {
	for (int i = 0; i < MAX_DEPTH - 2; i++) {
		killers[i][0] = killers[i + 2][0];
		killers[i][1] = killers[i + 2][1];
	}
	memset(&killers[MAX_DEPTH - 2], 0, 2 * sizeof(killers[0]));
}

int quiescence (Pos *pos, int depth, int alpha, int beta, int ply) {
	int side = pos->side;
	int king_sq = __builtin_ctzll(pos->bitboard[side ? 11 : 5]);
	int in_check = is_attacked(king_sq, side, pos);

	int original_alpha = alpha;
	int tt_score = 0;
	TTEntry *raw = tt_probe(pos->hash);
	TTEntry entry;
	if (raw) {
		entry = *raw;
		tt_score = entry.score;
		if (tt_score > 99000) tt_score -= ply;
		else if (tt_score < -99000) tt_score += ply;
		if (entry.flag == TT_EXACT) return tt_score;
		if (entry.flag == TT_LOWER && tt_score >= beta) return tt_score;
		if (entry.flag == TT_UPPER && tt_score <= alpha) return tt_score;
	}

	int stand_pat = (side == 0) ? eval_pos(pos) : -eval_pos(pos);
	if (!in_check) {
		if (raw) {
			if (entry.flag == TT_LOWER && tt_score > stand_pat)
				stand_pat = tt_score;
		}

		if (depth == 0) return stand_pat;
		if (stand_pat + 1000 < alpha) return alpha;
		if (stand_pat >= beta) return beta;
		if (stand_pat > alpha) alpha = stand_pat;
	} else {
		if (depth < -3) return stand_pat;
	}

	Move moves[256];
	int count;
	if (in_check) {
		if (side) count = get_b_moves(moves, pos);
		else count = get_w_moves(moves, pos);
	} else {
		if (side) count = get_b_captures(moves, pos);
		else count = get_w_captures(moves, pos);
	}
	
	int see_scores[count];
	for (int i = 0; i < count; i++) {
		if (moves[i].capture != -1) {
			see_scores[i] = see(pos, moves[i].to, values[moves[i].capture % 6], moves[i].from, values[moves[i].pieza % 6]);
		}
	} 

	sort_moves_quiescence (moves, count, pos, see_scores);

	int legal = 0;
	for (int i = 0; i < count; i++) {
		if (!in_check && moves[i].capture != -1) {
			if (see_scores[i] < 0) continue;
			int delta_margin = 200;
			if (stand_pat + values[moves[i].capture % 6] + delta_margin < alpha) continue;
		}

		Pos child = *pos;
		apply_move(&moves[i], &child);

		int ksq = __builtin_ctzll(child.bitboard[side ? 11 : 5]);
		if (is_attacked(ksq, side, &child)) continue;
		legal++;

		int eval = -quiescence(&child, depth - 1, -beta, -alpha, ply + 1);
		if (eval > alpha) alpha = eval;
		if (alpha >= beta) return beta;
	}

	if (in_check && legal == 0) return -100000 + ply;

	return alpha;
}

int negamax (Pos *pos, int depth, int ply, int alpha, int beta, Move *best_out, int null_allowed, int en_jaque) {
	nodes++;
	if (!(nodes & 2047)) {
		if (get_time_ms() >= deadline) interrupted = 1;
	}
	if (interrupted) return 0;

	if (null_allowed) {
		for (int i = rep_top - 3; i >= 0; i -= 2) {
			if (rep_stack[i] == pos->hash) return 0;
		}
	}

	if (depth == 0) return quiescence(pos, 5, alpha, beta, ply);

	int side = pos->side;
	int original_alpha = alpha;

	TTEntry *raw = tt_probe(pos->hash);
	int score = 0;
	TTEntry entry;
	if (raw) {
		entry = *raw;
		score = entry.score;
		if (score > 99000) score -= ply;
		else if (score < -99000) score += ply;
		if (entry.depth >= depth) {
			if (best_out && entry.best.from != entry.best.to)
				*best_out = entry.best;
			if (entry.flag == TT_EXACT) return score;
			if (entry.flag == TT_LOWER && score >= beta) return score;
			if (entry.flag == TT_UPPER && score <= alpha) return score;
		}
	}

	int non_pawn_material = __builtin_popcountll(
		pos->bitboard[side ? 7 : 1] | pos->bitboard[side ? 8 : 2] |
		pos->bitboard[side ? 9 : 3] | pos->bitboard[side ? 10 : 4]);

	int opp_king = __builtin_ctzll(pos->bitboard[side ? 5 : 11]);

	if (depth >= 3 && non_pawn_material && null_allowed && !en_jaque) {
		Pos null_pos = *pos;
		null_pos.side ^= 1;
		null_pos.hash ^= zob_turn;
		int old_ep_idx = (null_pos.en_passant == -1) ? 8 : (null_pos.en_passant & 7);
		null_pos.en_passant = -1;
		null_pos.hash ^= zob_ep[old_ep_idx];

		int reduction = 3 + depth / 6;
		int c = depth - 1 - reduction;
		if (c < 0) c = 0;
		int null_eval = -negamax(&null_pos, c, ply + 1, -beta, -beta + 1, NULL, 0, 0);
		
		if (null_eval >= beta) return beta;
	}

	Move moves[256];
	int total_moves;
	if (side) total_moves = get_b_moves(moves, pos);
	else total_moves = get_w_moves(moves, pos);
	Move *actual_move = moves;

	Move tt_best_move = {0};
	int has_tt_move = (raw != NULL && raw->best.from != raw->best.to);

	if (has_tt_move) {
		tt_best_move = entry.best;
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

	sort_moves(moves + has_tt_move, total_moves - has_tt_move, ply, pos);

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

		int da_jaque = is_attacked(opp_king, !side, &child);

		rep_stack[rep_top++] = child.hash;

		int eval;
		if (i == 0) {
			eval = -negamax(&child, depth - 1, ply + 1, -beta, -alpha, NULL, 1, da_jaque);
		} else {
			int reduction = 0;
			if (i >= 3 && !da_jaque && !en_jaque && depth >= 3 && actual_move->capture == -1) {
				reduction = lmr_table[depth][i];
				if (reduction < 1) reduction = 1;
				if (reduction > depth - 2) reduction = depth - 2;
			}
			eval = -negamax(&child, depth - 1 - reduction, ply + 1, -alpha - 1, -alpha, NULL, 1, da_jaque);
			if (eval > alpha && eval < beta) {
				eval = -negamax(&child, depth - 1, ply + 1, -beta, -alpha, NULL, 1, da_jaque);
			}
		}

		rep_top--;

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
		int score = en_jaque ? -(100000 - ply) : 0;
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

Move bot_move (int max_depth, long long time, Pos *pos) {
	deadline = get_time_ms() + time;
	interrupted = 0;
	nodes = 0;

	Move best_move = {0};
	Move candidate = {0};
	shift_killers();
	int prev_score = 0;

	int en_jaque = is_attacked(__builtin_ctzll(pos->bitboard[pos->side ? 11 : 5]), pos->side, pos);

	for (int i = 1; i <= max_depth; i++) {
		int saved_rep_top = rep_top;
		int score;
		for (int p = 0; p < 12; p++)
			for (int sq = 0; sq < 64; sq++)
				history[p][sq] >>= 1;
		if (i < 3) {
			score = negamax(pos, i, 0, -1000000, 1000000, &candidate, 1, en_jaque);
		} else {
			int delta = 50;
			int alpha = prev_score - delta;
			int beta = prev_score + delta;
			int alpha_delta = delta;
			int beta_delta = delta;
			while (1) {
				score = negamax(pos, i, 0, alpha, beta, &candidate, 1, en_jaque);
				if (interrupted) break;
				if (score <= alpha) {
					alpha_delta <<= 1;
					alpha = score - alpha_delta;
				}
				else if (score >= beta) {
					beta_delta <<= 1;
					beta = score + beta_delta;
				}
				else break;
			}
		}
		rep_top = saved_rep_top;
		if (interrupted) break;
		printf("info depth %d score cp %d nodes %lld\n", i, score, nodes);
		fflush(stdout);
		best_move = candidate;
		prev_score = score;
		if (score > 99000 || score < -99000) break;
	}
	if (best_move.from == best_move.to) {
		Move fallback_moves[256];
		int count = pos->side ? get_b_moves(fallback_moves, pos) : get_w_moves(fallback_moves, pos);
		for (int i = 0; i < count; i++) {
			Pos tmp = *pos;
			apply_move(&fallback_moves[i], &tmp);
			int ksq = __builtin_ctzll(tmp.bitboard[pos->side ? 11 : 5]);
			if (!is_attacked(ksq, pos->side, &tmp)) {
				best_move = fallback_moves[i];
				break;
			}
		}	
	}
	return best_move;
}








