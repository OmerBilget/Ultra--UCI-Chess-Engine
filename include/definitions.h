#ifndef DEFINITIONS_H
#define DEFINITIONS_H




#include   <stdlib.h>
#include  <stdint.h>
#include  <vector>
#include <stdio.h>
#include <atomic>



//*********************************
//debug

//if next line commented all debug disabled
//#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)){ \
printf("%s - Failed ",#n); \
printf("In File %s ",__FILE__); \
printf("At Line %d",__LINE__); \
exit(1);}
#endif // DEBUG
//*******************************


//definitions

#define NAME "TITAN"
#define AUTHOR "Marvin"


//fixed 64 bit and 32 bit numbers;
typedef uint64_t UINT_64;
typedef uint32_t UINT_32;


//board value meanings
#define  PAWN      1
#define  ROOK       2
#define  KNIGHT    3
#define  BISHOP    4
#define  QUEEN     5
#define  KING        6

#define  PAWN_INDEX        0
#define  ROOK_INDEX         1
#define  KNIGHT_INDEX      2
#define  BISHOP_INDEX      3
#define  QUEEN_INDEX       4
#define  KING_INDEX          5

#define  WHITE_PAWN      1
#define  WHITE_ROOK       2
#define  WHITE_KNIGHT    3
#define  WHITE_BISHOP    4
#define  WHITE_QUEEN     5
#define  WHITE_KING        6

#define  BLACK_PAWN      9
#define  BLACK_ROOK       10
#define  BLACK_KNIGHT    11
#define  BLACK_BISHOP    12
#define  BLACK_QUEEN     13
#define  BLACK_KING        14

#define EMPTY  0
#define OUT_OF_BOARD  -1

#define WHITE 0
#define BLACK 1
#define BOTH 2



//--xxx piece id
//xx--- 01 black 00 white
//01001 black pawn 9

//piece_color piece>>PIECE_ROTATE
//piece_type piece&PIECE_MASK

#define PIECE_MASK 0b111
#define PIECE_ROTATE 3


//10x12 board
#define HEIGHT 12
#define WIDTH 10
#define BOARD_LENGTH 120
#define BOARD_START 21

#define BOARD_ROW_LENGTH 8
#define HISTORY_LEN 2048
#define MAX_GAME_MOVES 2048

#define MAX_SEARCH_DEPTH 64
#define NO_MOVE 0
#define INFINITY 20000000
#define CHECK_MATE_SCORE 10000000
#define IS_MATE 9900000
#define FALSE 0
#define TRUE 1


#define WHITE_KING_CASTLE 1
#define WHITE_QUEEN_CASTLE 2
#define BLACK_KING_CASTLE 4
#define BLACK_QUEEN_CASTLE 8

#define OUB -1
#define NO_EP -400

#define MAX_MOVE_COUNT_PER_POSITION 256


//*************************************************
//structs and enums


//move struct stores move format and score
//score used for move ordering
typedef struct {
   UINT_32 id;
    int score;
}MOVE;


//list of moves
typedef struct movelist{
    MOVE moves[MAX_MOVE_COUNT_PER_POSITION];
    int counter;
}MOVELIST;

//transposition table flags
enum{ HF_NONE,HF_ALPHA,HF_BETA,HF_EXACT};
//pv table
typedef struct hash_entry{
    UINT_64 position_key;
    UINT_32 mov;
    int score;
    int depth;
    int flags;
}HASH_ENTRY;

typedef struct hash_table{
    std::vector<HASH_ENTRY> table;
    unsigned int number_of_entries;
    int new_write;
	int over_write;
	int hit;
	int cut;
}HASH_TABLE;





//this struct used for storing non-recoverable information into an array so moves can be make or unmake
typedef struct undo_move{
    UINT_32 move_id;
    int castle_permissions;  // 0000  no castle 0001 player down 1000 player up
    int fiftyMoveCounter;
    int enpassant_square;
    UINT_64 position_key;
}UNDO;


//main board struct
typedef struct b_struct{
      int board[BOARD_LENGTH];

      int side;
      int enPassant_square;
      int fiftyMoveCounter;
      int ply;


      int piece_count[2][6]; // WHITE PAWN  piece_count[WHITE][PAWN]
      int materials[2];
      UINT_64 pawns[3];//black white both
      int castle_permissions;  //0001     WK 0001 / WQ 0010 / BK 0100 / BQ 1000
      int piece_list[2][6][10];

      UINT_64 position_key;//unique position key for transposition table

      UNDO history[MAX_GAME_MOVES]; //array of past moves that holds non recoverable information
      int history_counter; //counter that hold number of history array

       //hash_table
       HASH_TABLE hash_table;
       UINT_32 pv_array[MAX_SEARCH_DEPTH];

       //history heuristics
       //every time move improves alpha increment by depth value
       UINT_32 search_history[2][6][BOARD_LENGTH];
       //killer move heuristics used for move ordering
       //beta cutoff non captures
       UINT_32 search_killer_moves[2][MAX_SEARCH_DEPTH];
}BOARD_STRUCT;

//search info struct
//search parameters given by gui
typedef struct search_information{
    UINT_64 start_time;
    UINT_64 end_time;
    int depth_limit;
    int time_limit;
    bool time_set;
    bool depth_set;
    bool moves_to_go;
    bool infinite; //if set to true search never stops until stop command
    unsigned long nodes; //all nodes searched

    //parallel search atomic instructions
    std::atomic<bool> quit;
    std::atomic<bool> stopped; //search stop

    //used for measure move ordering rate fail_high_first/fail_high
    float fail_high;
    float fail_high_first;

}SEARCH_INFORMATION;

//**********************************************




//used for move generation
//this moves determines move patterns for each piece
const int knight_direction[8]={-8,-19,-21,-12,8,19,21,12};
const int rook_direction[4]={-1,-10,1,10};
const int bishop_direction[4]={-9,-11,11,9};
const int king_direction[8]={-1,-10,1,10,-9,-11,11,9};
const int queen_direction[8]={-1,-10,1,10,-9,-11,11,9};


//piece values used in material score
//pawn rook knight bishop queen king
//const int piece_values[6]={100,550,325,325,1000,50000};
const int piece_values[6]={200,1100,650,650,2000,500000};

//used in legal move generation
//if king is checked by sliding piece king cant move opposite direction to the sliding piece
const int rook_direction_mirrored[4]={1,10,-1,-10};
const int bishop_direction_mirrored[4]={9,11,-11,-9};


//castling permission arrays
//castle permission&castle_permission_array[square]
//is king is moved or rook is moved this and operation updates castle permissions
const int CASTLE_PERMISSION_ARRAY[120]={
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,   7, 15, 15, 15,   3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

//********************************************************
//board conversions


const int BOARD_64_To_120[64]={
    21,22,23,24,25,26,27,28,
    31,32,33,34,35,36,37,38,
    41,42,43,44,45,46,47,48,
    51,52,53,54,55,56,57,58,
    61,62,63,64,65,66,67,68,
    71,72,73,74,75,76,77,78,
    81,82,83,84,85,86,87,88,
    91,92,93,94,95,96,97,98
};

const int BOARD_120_To_64[120]={
    OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,
    OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,
    OUB,0     ,1      ,2     ,3     ,4     ,5     ,6      ,7     ,OUB,
    OUB,8     ,9      ,10   ,11   ,12   ,13   ,14   ,15    ,OUB,
    OUB,16   ,17    ,18   ,19   ,20   ,21   ,22   ,23    ,OUB,
    OUB,24   ,25    ,26   ,27   ,28   ,29   ,30   ,31    ,OUB,
    OUB,32   ,33   ,34    ,35   ,36   ,37   ,38   ,39    ,OUB,
    OUB,40   ,41   ,42   ,43    ,44   ,45   ,46    ,47   ,OUB,
    OUB,48   ,49   ,50   ,51   ,52    ,53   ,54    ,55   ,OUB,
    OUB,56    ,57  ,58   ,59   ,60    ,61   ,62   ,63    ,OUB,
    OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,
    OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB,OUB
};


//mirror vertically
const int mirror_64[64]=
{
    56,57,58,59,60,61,62,63,
    48,49,50,51,52,53,54,55,
    40,41,42,43,44,45,46,47,
    32,33,34,35,36,37,38,39,
    24,25,26,27,28,29,30,31,
    16,17,18,19,20,21,22,23,
     8 ,  9,10,11,12,13,14,15,
     0 ,  1,  2,  3,  4,  5,  6,  7
};

const int mirror_120[120]={
    110,111,112,113,114,115,116,117,118,119,
    100,101,102,103,104,105,106,107,108,109,
    90,91,92,93,94,95,96,97,98,99,
    80,81,82,83,84,85,86,87,88,89,
    70,71,72,73,74,75,76,77,78,79,
    60,61,62,63,64,65,66,67,68,69,
    50,51,52,53,54,55,56,57,58,59,
    40,41,42,43,44,45,46,47,48,49,
    30,31,32,33,34,35,36,37,38,39,
    20,21,22,23,24,25,26,27,28,29,
    10,11,12,13,14,15,16,17,18,19,
    0  ,1  ,2  ,  3,  4,  5,  6,  7,  8,  9
};

//***************************************



/*
move structure

from
to
capture
ep Move
Promote move
pawn start
castling Move

0000 0000 0000 0000 0000 0111 1111  //from 0-7     get : mov& 0x7F
0000 0000 0000 0011 1111 1000 0000  //to 8-14        get : mov>>7 0x7F
0000 0000 0011 1100 0000 0000 0000  //captured     get : mov>>14 0xF
0000 0000 0100 0000 0000 0000 0000  //ep               get : 0x40000
0000 0000 1000 0000 0000 0000 0000  //pawn start  get : 0x80000
0000 1111 0000 0000 0000 0000 0000  //promote     get : mov>>20 0XF
0001 0000 0000 0000 0000 0000 0000  //castling       get : 0x1000000
*/


#define FROM_ENCODE(n)  ((n) & 0x7F)
#define TO_ENCODE(n)  (((n)>>7)& 0x7F)
#define CAPTURE(n) (((n)>>14)& 0xF)
#define PROMOTE(n) (((n)>>20)& 0xF)

#define FLAG_EP 0x40000
#define FLAG_PAWN_START 0x80000
#define FLAG_CASTLE 0x1000000

#define FLAG_IS_CAPTURED  0x7C000
#define FLAG_IS_PROMOTED 0xF00000

#define CREATE_MOVE(from,to,capture,promotion,flag) ((from) | ((to) << 7) |  ((capture)<<14) | ((promotion) <<20) |  (flag))




// most valuable victim least valuable attacker table
//used for move ordering
//a pawn capturing a queen is most valuable
// mvvlva[attacker][victim]
const int mvvlva[6][6]={
{105,405,205,305,505,0},
{102,402,202,302,502,0},
{104,404,204,304,504,0},
{103,403,203,303,503,0},
{101,401,201,301,501,0},
{0,0,0,0,0,0}
};

//********************************************************************
//bitboards

#define SET_BIT_ZERO(bb,index)   (bb&=(~(1ULL<<index)))
#define SET_BIT_ONE(bb,index)     (bb|=(1ULL<<index))



#endif // DEFINITIONS_H
