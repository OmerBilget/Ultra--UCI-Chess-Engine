#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "definitions.h"
#include   "SquareAttacked.h"
#include "Makemove.h"
inline void add_quiet_move(BOARD_STRUCT*,UINT_32,MOVELIST*);
inline void add_capture_move(BOARD_STRUCT*,UINT_32,MOVELIST*);
inline void add_ep_move(BOARD_STRUCT*,UINT_32,MOVELIST*);

inline void add_white_pawn_move_capture(BOARD_STRUCT*,const int,const int,const int,MOVELIST*);
inline void add_white_pawn_move(BOARD_STRUCT*,const int ,const int ,MOVELIST*);

inline void add_black_pawn_move_capture(BOARD_STRUCT*,const int,const int,const int,MOVELIST*);
inline void add_black_pawn_move(BOARD_STRUCT*chessboard ,const int,const int,MOVELIST*);

void generate_all_moves(BOARD_STRUCT*,MOVELIST*);

void generate_legal_moves(BOARD_STRUCT* ,MOVELIST*);
inline void calculate_pins(int [],int [],int&,int[] ,int,BOARD_STRUCT* );

bool is_move_exists(BOARD_STRUCT* ,UINT_32);
bool is_move_exists_version_with_movelist(BOARD_STRUCT* ,MOVELIST* ,UINT_32);

void generate_legal_capture_moves(BOARD_STRUCT*,MOVELIST* );
#endif // MOVEGEN_H
