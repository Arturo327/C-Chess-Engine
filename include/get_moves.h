#ifndef GET_MOVES_H
#define GET_MOVES_H

#include <stdint.h>
#include "movegen.h"
#include "types.h"

int get_w_moves(Move *moves, Pos *pos);
int get_b_moves(Move *moves, Pos *pos);

#endif
