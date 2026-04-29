#ifndef MAKE_MOVE_H
#define MAKE_MOVE_H

#include <stdint.h>

#include "types.h"
#include "zobrist.h"

void apply_move(Move *move, Pos *pos);

#endif
