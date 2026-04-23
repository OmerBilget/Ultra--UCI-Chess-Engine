#ifndef SEARCH_H
#define SEARCH_H

#include "definitions.h"
#include "Evaluation.h"
#include "Timer.h"
#include "PV_Table.h"
#include "IO.h"
#include "Movegen.h"
#include <iostream>


inline bool is_repetition(const BOARD_STRUCT *);
void clear_for_search(BOARD_STRUCT* ,SEARCH_INFORMATION *);
void search_position(BOARD_STRUCT* ,SEARCH_INFORMATION * );
int alpha_beta(int ,int,int,BOARD_STRUCT*,SEARCH_INFORMATION *,bool);
int quiescense_search(int ,int,BOARD_STRUCT* ,SEARCH_INFORMATION *);
inline void pick_next_move(int,MOVELIST*);
inline void check_timer(SEARCH_INFORMATION* );
#endif // SEARCH_H
