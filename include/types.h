#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>

typedef struct {
    unsigned int pieza : 4;
    int capture : 5;
    unsigned int from : 6;
    unsigned int to : 6;
} Move;

typedef struct {
    uint64_t bitboard[12];
    int board[64];
    int en_passant;
    unsigned int castle : 4;
} Pos;

#endif
