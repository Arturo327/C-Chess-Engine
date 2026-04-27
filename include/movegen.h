#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdint.h>
#include "init.h"
#include "types.h"

int is_attacked(int sq, int by_withe, Pos *pos);
uint64_t get_w_pawn_captures(int from, uint64_t enemy, int en_passant);
uint64_t get_b_pawn_captures(int from, uint64_t enemy, int en_passant);
uint64_t get_w_pawn_moves(int from, uint64_t occupied);
uint64_t get_b_pawn_moves(int from, uint64_t occupied);
uint64_t get_rook_attacks(int from, uint64_t friendly, uint64_t enemy);
uint64_t get_alfil_attacks(int from, uint64_t friendly, uint64_t enemy);
uint64_t get_queen_attacks(int from, uint64_t friendly, uint64_t enemy);

int get_short_castle_w (uint64_t occupied, Move *out, Pos *pos);
int get_long_castle_w (uint64_t occupied, Move *out, Pos *pos);
int get_short_castle_b (uint64_t occupied, Move *out, Pos *pos);
int get_long_castle_b (uint64_t occupied, Move *out, Pos *pos);

#endif
