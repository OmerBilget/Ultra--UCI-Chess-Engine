#include "Validate.h"
#include "HashKey.h"
bool is_square_on_board(const int square)
{
    return(BOARD_120_To_64[square]!=OUB);
}
bool is_side_valid(const int side)
{
    return (side==0 || side==1);
}
bool is_piece_valid(const int piece)
{
    int piece_type=piece&PIECE_MASK;
    return(piece_type>=1 && piece_type<=6 && piece<=15);
}

bool validate_board(BOARD_STRUCT* chessboard)
{
    int tmp_piece_number[2][6];

    int tmp_material[2];
    int piece;
    int square;
    UINT_64 tmp_pawn[3]={0};
    //check is list true
    for(int i=0; i<2; i++)
    {
        for(int j=0; j<6; j++)
        {
            for(int k=0;k<chessboard->piece_count[i][j]; k++)
            {
                square=chessboard->piece_list[i][j][k];
                ASSERT(chessboard->board[square]==((j+1)+(i*8)));
                  if(j==PAWN_INDEX){
                     SET_BIT_ONE(tmp_pawn[i],BOARD_120_To_64[square]);
                     SET_BIT_ONE(tmp_pawn[BOTH],BOARD_120_To_64[square]);
                }
            }
        }
    }
    for(int i=0;i<3;i++){
        ASSERT(tmp_pawn[i]==chessboard->pawns[i]);
    }
    //clear array
   for(int i=0;i<2;i++){
        for(int j=0;j<6;j++){
           tmp_piece_number[i][j]=0;
        }
    }
    tmp_material[0]=0;
    tmp_material[1]=0;
    //fill tmp piece count list and calculate material score
    int sq120;
    int piece_type,player;
    for(int i=0; i<64; i++)
    {
        sq120=BOARD_64_To_120[i];
        piece=chessboard->board[sq120];
        if(piece!=OUT_OF_BOARD && piece!=EMPTY)
        {
            player=piece>>PIECE_ROTATE; //white or black
            piece_type=piece&PIECE_MASK;

            tmp_piece_number[player][piece_type-1]++;
            tmp_material[player]+=piece_values[piece_type-1];
        }
    }
    //check piece counts
    for(int i=0;i<2;i++){
        for(int j=0;j<6;j++){
            ASSERT((tmp_piece_number[i][j]==chessboard->piece_count[i][j]));
        }
    }

    //check materials
    ASSERT(tmp_material[0]==chessboard->materials[0]);
    ASSERT(tmp_material[1]==chessboard->materials[1]);
    ASSERT(chessboard->side==0 || chessboard->side==1);
    ASSERT(chessboard->position_key==generate_position_key(chessboard)  );
    return true;
}
