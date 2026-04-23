#ifndef XOSHIRO256_H
#define XOSHIRO256_H

//random number generator with four seeds

#include "definitions.h"
#include <stdint.h>


#define KEY1  1233294123
#define KEY2 30409934322213
#define KEY3  23472994823
#define KEY4  90342342
struct xoshiro256ss_state {
	uint64_t s[4];
};
extern xoshiro256ss_state random_state;
uint64_t xoshiro256ss(xoshiro256ss_state*);
void feed_states(uint64_t ,uint64_t ,uint64_t,uint64_t ,xoshiro256ss_state* );


#endif // XOSHIRO256_H
