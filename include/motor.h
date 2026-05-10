#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

#include "get_captures.h"
#include "get_moves.h"
#include "make_move.h"
#include "eval.h"
#include "types.h"
#include "sort.h"
#include "see.h"
#include "tt.h"
#include "zobrist.h"
#include "bitboards.h"

#define REP_STACK_SIZE 1024
extern uint64_t rep_stack[REP_STACK_SIZE];
extern int rep_top;

int quiescence (Pos *pos, int depth, int alpha, int beta);
int negamax (Pos *pos, int depth, int ply, int alpha, int beta, Move *best_out, int null_allowed);
Move bot_move(int depth, long long movetime, Pos *pos);

#endif
