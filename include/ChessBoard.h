#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include   "definitions.h"
#include "HashKey.h"


void init_board(BOARD_STRUCT* );
void reset_board(BOARD_STRUCT* );
void fill_lists(BOARD_STRUCT *);
void fill_materials(BOARD_STRUCT * );
void mirror_board(BOARD_STRUCT* );


#endif // CHESSBOARD_H
