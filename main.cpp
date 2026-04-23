#include <iostream>
#include "definitions.h"
#include "UCI_Protocol.h"
#include "Fen.h"
#include "Bitboard.h"
#include "ChessBoard.h"
#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQ - 0 1"
#define WAC2 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1"
 #include "Bitboard_Masks.h"


void init_pass_bitboard(UINT64 bitboard[64]);
int main()
{


   //UINT64 bb[64]={0};
    BOARD_STRUCT chessboard;
    SEARCH_INFORMATION search_information;
    search_information.quit=false;
    init_board(&chessboard);
    reset_board(&chessboard);
    fill_hashkeys(&random_state);
    init_hash_table(&chessboard.hash_table);
    printf("v.1.0.9.5\n");


   /*
    parse_fen_string("1k1rqqr1/8/8/8/Q7/2Q5/3R4/1K1R4 w - - 0 1",&chessboard);

     int score=evaluation(&chessboard);
     print_chessboard(chessboard.board);

     mirror_board(&chessboard);
     print_chessboard(chessboard.board);
     int negative_score=evaluation(&chessboard);
     printf("score: %d  negative score %d\n",score,negative_score);
   */
   //init_pass_bitboard(bb);

  /*
   for(int i=0;i<64;i++){

           printf("\n");
           print_bitboard(direction_masks[SOUTH][i]);

   }
  */



   uci_loop(&chessboard,&search_information);
    return 0;
}


void init_pass_bitboard(UINT64 bitboard[64]){
    int h;
    int v;
    int dir[8][2]={{-1,-1},{-1,1},{1,-1},{1,1},{0,-1},{0,1},{-1,0},{1,0}};
    for(int i=0;i<64;i++){
        h=i%8;
        v=i/8;


       if(h!=0 && v!=7){
            SET_BIT_ONE(bitboard[i],( (v+1) *8  )+(h-1));
       }
        if(h!=7 && v!=7){
            SET_BIT_ONE(bitboard[i],( (v+1) *8  )+(h+1));
       }





    }




            for(int i=0;i<64;i++){
            UINT_64 b=king_inner_ring[i]|king_middle_ring[i];
                //printf("%d\n",i);
                printf("%#016llX,\n",b);
               //print_bitboard(b);
              // printf("\n");
            }


}


