#ifndef EVALUATION_H
#define EVALUATION_H



#include "definitions.h"
#include "Bitboard_Masks.h"
#include "Bitboard.h"

//*************************************************

//piece tables used for evaluation

const int pawn_score_table[64] =
{
        80,  80,  80,  90,  90,  80,  80,  80,
        30,  30,  30,  40,  40,  30,  30,  30,
        20,  20,  20,  30,  30,  30,  20,  20,
        10,  10,  10,  20,  20,  10,  10,  10,
         5,    5,     7,  20,  20,    5,    5,    5,
         0,    0,     2,    5,    5,    0,    0,    0,
         0,    0,     0, -30, -40,   0,    0,    0,
         0,    0,     0,    0,     0,    0,    0,    0
 };

// knight positional score
const int knight_score_table[64]=
{
    -5,   0,    0,    0,    0,   0,   0,  -5,
    -5,   0,    0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,  5,  -5,
    -5, 10,  20,  30,  30,  20, 10, -5,
    -5, 10,  20,  30,  30,  20, 10, -5,
    -5,   5,  20,  10,  10,  20,  5,  -5,
    -5,   0,   0,    0,    0,    0,   0,  -5,
    -5, -10,  0,    0,   0,    0,-10,  -5
};

// bishop positional score
const int bishop_score_table[64] =
{
       -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
};

// rook positional score
const int rook_score_table[64] =
{
    25, 25,  25,  25,  25,  25,  25,  25,
    25, 25,  25,  25,  25,  25,  25,  25,
     0,    0,    5,  10,  10,    5,    0,    0,
     0,    0,    5,  10,  10,    5,    0,    0,
     0,    0,    5,  10,  10,    5,    0,    0,
     0,    0,    5,  10,  10,    5,    0,    0,
     0,    0,    5,  10,  10,    5,    0,    0,
     0,    0,    0,  10,  10,    0,    0,    0

};



//king endgame
const int KingE[64] = {
-50	,	-10	,	0	   ,	0	   ,	0	   ,	0	   ,	-10	,	-50	,
-10  ,	0	   ,	10	,	10	,	10	,	10	,	0	   ,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	  0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	  0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,     0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	  0	,
-10,	      0	,	10	,	10	,	10	,	10	,	0	   ,	-10	,
-50	,	-10	,	0  	,	 0 	,	0  	,	0  	,	-10	,	-50
};

//king opening
const int KingO[64] = {
	  0    ,	5  	,	5  	,	-10	,	-10	,	0	    ,	30	,	  5	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70
};
const int pawn_passed_score[8]={0,5,10,25,40,70,150,200};

const int connected_pawn_passed_score[8]={0,5,15,50,100,200,500};

const int mobility_multiplier[6]={3,6,4,4,7,1};
const int defend_multiplier[6]={6,2,4,3,2,1};
#define PAWN_ISOLATED_SCORE -10
#define CONNECTED_PAWN_BASE_SCORE 5
#define ROOK_OPEN_FILE 15
#define ROOK_SEMI_OPEN_FILE 7
#define QUEEN_OPEN_FILE 8
#define QUEEN_SEMI_OPEN_FILE 4
#define BISHOP_PAIR_SCORE 40
#define DOUBLED_PAWN_SCORE -15
//if bishop attack path is blocked by friendly pawn this is bad score calculated per pawn
#define BAD_BISHOP_SCORE -5
#define END_GAME_MATERIAL (1*piece_values[ROOK_INDEX]+2*piece_values[KNIGHT_INDEX] +2*piece_values[PAWN_INDEX])


#define CENTER_CONTROL_PAWN_SCORE 15
#define CENTER_CONTROL_PIECES_SCORE 11


const int SafetyTable[100] = {
    0,  1,   1,   2,   3,   5,   8,   9,  12,  15,
  18,  22,  26,  30,  35,  39,  44,  50,  56,  62,
  68,  75,  82,  85,  89,  97, 105, 113, 122, 131,
 140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
 260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
 377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
 494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};

const int king_attack_index[6]={2,3,2,2,5,1};


#define PAWN_SHIELD_SCORE 15
#define NEARBY_PIECES_BONUS 2

int evaluation(const BOARD_STRUCT* );
bool is_material_draw(const BOARD_STRUCT* );
void create_attack_tables(const BOARD_STRUCT* chessboard ,const UINT_64 piece_bitboard[2][6],UINT_64 attack_tables[2][6],const UINT_64 all_pieces[],UINT_64 defend_tables[2][6]);
#endif // EVALUATION_H
