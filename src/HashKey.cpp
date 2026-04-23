#include "HashKey.h"

UINT_64 piece_keys[2][6][120]; //a key for every color ,piece type and board position combination
UINT_64 side_key; // key used hash white turn or black turn
UINT_64 castle_key[16]; //key used castling
UINT_64 EP_key[120];  // key used ep positions




void fill_hashkeys(xoshiro256ss_state* state)
{
    feed_states(KEY1,KEY2,KEY3,KEY4,state);
    int i,j,k;
    for(i=0; i<2; i++)
    {
        for(j=0; j<6; j++)
        {
            for(k=0; k<120; k++)
            {
                piece_keys[i][j][k]=xoshiro256ss(state);
            }
        }
    }


    side_key=xoshiro256ss(state);

    for(i=0; i<16; i++)
    {
        castle_key[i]=xoshiro256ss(state);
    }

    for(i=0; i<120; i++)
    {
        EP_key[i]=xoshiro256ss(state);
    }
}


UINT_64 generate_position_key(BOARD_STRUCT* chessboard)
{

    UINT_64 final_key=0;
    int piece=EMPTY;
    for(int i=0; i<120; i++)
    {
        piece=chessboard->board[i];
        if(piece!=OUT_OF_BOARD &&  piece!=EMPTY)
        {
            final_key^=piece_keys[piece>>PIECE_ROTATE][(piece&PIECE_MASK)-1][i];
        }
    }

    if(chessboard->side==WHITE)
    {
        final_key^=side_key;
    }

    if(chessboard->enPassant_square!=NO_EP)
    {
        ASSERT(chessboard->enPassant_square>= 0 &&  chessboard->enPassant_square<=BOARD_LENGTH);
        final_key^=EP_key[chessboard->enPassant_square];
    }
    ASSERT(chessboard->castle_permissions>=0 && chessboard->castle_permissions<=15)
    final_key^=castle_key[chessboard->castle_permissions];

    return final_key;

}


void print_hashkeys(){
    printf("\n piece_keys\n\n");
    int i,j,k;
    for(i=0; i<2; i++)
    {
        for(j=0; j<6; j++)
        {
            for(k=0; k<120; k++)
            {
                printf("%d %d %d  -  %llu\n",i,j,k,piece_keys[i][j][k]);
            }
        }
    }

    printf("\n side key : %llu \n\n",side_key);
    printf("castle keys\n");
    for(i=0; i<16; i++)
    {
         printf("%d -  %llu\n",i,castle_key[i]);
    }
    printf("\n ep_keys \n");
    for(i=0; i<120; i++)
    {
          printf("%d -  %llu\n",i,EP_key[i]);
    }
}

