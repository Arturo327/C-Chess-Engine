#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>

extern int values[6];

typedef struct {
	unsigned int from : 6;
	unsigned int to : 6;
	int capture : 5;
	unsigned int pieza : 4;
} Move;

typedef struct {
	uint64_t bitboard[12];
	int board[64];
	uint64_t hash;
	int8_t en_passant;
	unsigned int castle : 4;
	unsigned int side : 1;
} Pos;

#endif
