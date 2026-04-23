#include "Xoshiro256.h"
#include <stdint.h>
xoshiro256ss_state random_state;

uint64_t rol64(uint64_t x, int k)
{
	return (x << k) | (x >> (64 - k));
}

uint64_t xoshiro256ss(xoshiro256ss_state *state)
{
	uint64_t *s = state->s;
	uint64_t const result = rol64(s[1] * 5, 7) * 9;
	uint64_t const t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;
	s[3] = rol64(s[3], 45);

	return result;
}

void feed_states(uint64_t s0,uint64_t s1,uint64_t s2,uint64_t s3,xoshiro256ss_state* state){
    state->s[0]=s0;
    state->s[1]=s1;
    state->s[2]=s2;
    state->s[3]=s3;
}
