#ifndef VALIDATE_H
#define VALIDATE_H

#include "definitions.h"
bool is_square_on_board(const int );
bool is_side_valid(const int);
bool is_piece_valid(const int);
bool validate_board(BOARD_STRUCT*);
#endif // VALIDATE_H
