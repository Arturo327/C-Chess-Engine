#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "types.h"
#include "zobrist.h"

extern uint64_t horse_moves[64];
extern uint64_t king_moves[64];

void generate_horse_table(void);
void generate_king_table(void);
void reset(Pos *pos);

#endif
