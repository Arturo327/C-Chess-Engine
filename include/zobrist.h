#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <stdint.h>
#include "types.h"

extern uint64_t zob_pieces[12][64];
extern uint64_t zob_turn;
extern uint64_t zob_castle[16];
extern uint64_t zob_ep[9];

void init_zobrist(void);
uint64_t compute_hash(Pos *pos);

#endif
