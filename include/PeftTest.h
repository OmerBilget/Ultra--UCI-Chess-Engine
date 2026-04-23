#ifndef PEFTTEST_H
#define PEFTTEST_H

#include "definitions.h"
#include "Makemove.h"
#include "Movegen.h"

extern unsigned long long leaf_nodes;
void perft_test(int depth,BOARD_STRUCT* chessboard);
void divide_perft_test(int depth ,BOARD_STRUCT *chessboard);
void perft_test_legal_moves(int depth,BOARD_STRUCT* chessboard);
void divide_perft_test_legal_moves(int depth ,BOARD_STRUCT *chessboard);
void display(int );
#endif // PEFTTEST_H
