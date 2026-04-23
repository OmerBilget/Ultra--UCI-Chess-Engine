#ifndef MAKEMOVE_H
#define MAKEMOVE_H

#include "definitions.h"
#include "HashKey.h"
#include   "SquareAttacked.h"
inline void clear_piece(const int ,BOARD_STRUCT*);
inline void add_piece(const int,const int ,BOARD_STRUCT*);
inline void move_piece(const int ,const int ,BOARD_STRUCT* );
bool make_move(UINT_32 ,BOARD_STRUCT* );
void make_move_legal(UINT_32 ,BOARD_STRUCT*);
void take_move(BOARD_STRUCT*);
void make_null_move(BOARD_STRUCT* );
void take_null_move(BOARD_STRUCT* );
#endif // MAKEMOVE_H
