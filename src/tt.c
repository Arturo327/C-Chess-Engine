#include "tt.h"
#include <string.h>

TTCluster tt[TT_SIZE];

void tt_clear(void) {
	memset(tt, 0, sizeof(tt));
}

TTEntry *tt_probe(uint64_t hash) {
	TTCluster *cluster = &tt[hash & (TT_SIZE - 1)];
	if (cluster->bucket[0].key == hash) return &cluster->bucket[0];
		if (cluster->bucket[1].key == hash) return &cluster->bucket[1];
	return NULL;
}

void tt_store(uint64_t hash, int score, int depth, int flag, Move *best) {
	TTCluster *cluster = &tt[hash & (TT_SIZE - 1)];
	TTEntry *depth_slot  = &cluster->bucket[0];
	TTEntry *always_slot = &cluster->bucket[1];

	if (depth_slot->key == hash) {
		if (depth >= depth_slot->depth) {
			depth_slot->score = score;
			depth_slot->depth = depth;
			depth_slot->flag  = flag;
			if (best) depth_slot->best = *best;
		}
		return;
	}
	if (always_slot->key == hash) {
		always_slot->score = score;
		always_slot->depth = depth;
		always_slot->flag  = flag;
		if (best) always_slot->best = *best;
		return;
	}
	if (depth_slot->key == 0 || depth >= depth_slot->depth) {
		*always_slot = *depth_slot;
		depth_slot->key = hash;
		depth_slot->score = score;
		depth_slot->depth = depth;
		depth_slot->flag = flag;
		depth_slot->best = best ? *best : (Move){0};
	} else {
		always_slot->key = hash;
		always_slot->score = score;
		always_slot->depth = depth;
		always_slot->flag = flag;
		always_slot->best = best ? *best : (Move){0};
	}
}

