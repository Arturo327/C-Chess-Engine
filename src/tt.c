#include "tt.h"
#include <string.h>

TTEntry tt[TT_SIZE];

void tt_clear(void) {
	memset(tt, 0, sizeof(tt));
}

TTEntry *tt_probe(uint64_t hash) {
	TTEntry *e = &tt[hash % TT_SIZE];
	if (e->key == hash) return e;
	return NULL;
}

void tt_store(uint64_t hash, int score, int depth, int flag, Move *best) {
	TTEntry *e = &tt[hash % TT_SIZE];
	if (e->key == 0 || e->depth <= depth) {
		e->key = hash;
		e->score = (int32_t)score;
		e->depth = (int8_t)depth;
		e->flag = flag;
		if (best) e->best = *best;
	}
}
