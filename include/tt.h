#ifndef TT_H
#define TT_H

#include <stdint.h>
#include "types.h"

#define TT_SIZE (1 << 22)

#define TT_EXACT 0
#define TT_LOWER 1
#define TT_UPPER 2

typedef struct {
	uint64_t key;
	int32_t score;
	int8_t depth;
	uint8_t flag;
	Move best;
} TTEntry;

extern TTEntry tt[TT_SIZE];

void tt_clear(void);
TTEntry *tt_probe(uint64_t hash);
void tt_store(uint64_t hash, int score, int depth, int flag, Move *best);

#endif
