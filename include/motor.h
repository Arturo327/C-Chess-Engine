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

int quiescence_w (Pos *pos, int depth, int alpha, int beta);
int quiescence_b (Pos *pos, int depth, int alpha, int beta);

int search_moves_withe (Pos *pos, int depth, int alpha, int beta, Move *best_out);
int search_moves_black (Pos *pos, int depth, int alpha, int beta, Move *best_out);

Move bot_move(int depth, int withe, Pos *pos);

#endif
