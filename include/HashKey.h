#ifndef HASHKEY_H
#define HASHKEY_H


#include "definitions.h"
#include  "Xoshiro256.h"


//global variables
extern UINT_64 piece_keys[2][6][120];
extern UINT_64 side_key;
extern UINT_64 castle_key[16];
extern UINT_64 EP_key[120];


void fill_hashkeys(xoshiro256ss_state* );
void print_hashkeys();
UINT_64 generate_position_key(BOARD_STRUCT* );




#endif // HASHKEY_H
