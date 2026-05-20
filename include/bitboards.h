#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <stdint.h>

extern uint64_t rook_attack_table[64][4096];
extern uint64_t alfil_attack_table[64][512];
extern uint64_t rook_magics[64];
extern uint64_t alfil_magics[64];
extern uint64_t rook_masks[64];
extern uint64_t alfil_masks[64];
extern int rook_bits[64];
extern int alfil_bits[64];
extern uint64_t pawn_attacks[2][64];

void init_magics(void);

#endif
