#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

#include "get_captures.h"
#include "get_moves.h"
#include "make_move.h"
#include "eval.h"
#include "types.h"
#include "sort.h"
#include "tt.h"
#include "zobrist.h"

int quiescence (Pos *pos, int depth, int alpha, int beta);
int negamax (Pos *pos, int depth, int ply, int alpha, int beta, Move *best_out);
Move bot_move(int depth, Pos *pos);

#endif
