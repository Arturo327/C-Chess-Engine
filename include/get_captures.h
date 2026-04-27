#ifndef GET_CAPTURES_H
#define GET_CAPTURES_H

#include <stdint.h>
#include "movegen.h"
#include "types.h"

int get_w_captures(Move *moves, Pos *pos);
int get_b_captures(Move *moves, Pos *pos);

#endif
