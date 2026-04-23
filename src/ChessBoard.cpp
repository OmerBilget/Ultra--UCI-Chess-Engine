#include "ChessBoard.h"
#include  "Validate.h"

void init_board(BOARD_STRUCT* chessboard)
{
    //define borders
    for(int i=0; i<HEIGHT; i++)
    {
        for(int j=0; j<WIDTH; j++)
        {
            if(i<2 || i>HEIGHT-3 || j<1 || j>WIDTH-2)
            {
                chessboard->board[i*WIDTH+j]=OUT_OF_BOARD;
            }
        }
    }
    for(int i=2; i<HEIGHT-2; i++)
    {
        for(int j=1; j<WIDTH-1; j++)
        {
            chessboard->board[i*WIDTH+j]=EMPTY;
        }
    }
}

void reset_board(BOARD_STRUCT* chessboard){
    int i=0;
    int j=0;
    init_board(chessboard);
    //reset piece lists
    for(i=0;i<2;i++){
        for(j=0;j<6;j++){
            chessboard->piece_count[i][j]=0;
            for(int k=0;k<10;k++){
                chessboard->piece_list[i][j][k]=EMPTY;
            }
        }
    }
    //reset material score
    chessboard->materials[0]=0;
    chessboard->materials[1]=0;
    //reset pawn bitboards
    for(i=0;i<3;i++){
        chessboard->pawns[i]=0;
    }
    //reset remaining information
    chessboard->side=0;
    chessboard->enPassant_square=NO_EP;
    chessboard->fiftyMoveCounter=0;
    chessboard->ply=0;
    chessboard->history_counter=0;
    chessboard->castle_permissions=0;
    chessboard->position_key=0;
}


//fills piece lists and pawn bitboards
void fill_lists(BOARD_STRUCT *  chessboard){
   int i,j,k;
    //reset list
    for(i=0;i<2;i++){
        for(j=0;j<6;j++){
            chessboard->piece_count[i][j]=0;
            for(k=0;k<10;k++){
                chessboard->piece_list[i][j][k]=EMPTY;
            }
        }
    }
    //reset pawn bitboard
    for( i=0;i<3;i++){
        chessboard->pawns[i]=0;
    }
    int square;
    int piece_type,piece_color;
    for(i=0;i<BOARD_LENGTH;i++){
        square=chessboard->board[i];
        if(square!=OUT_OF_BOARD && square!=EMPTY){
            piece_color=square>>PIECE_ROTATE; //white or black
            piece_type=square&PIECE_MASK;
            chessboard->piece_list[piece_color][piece_type-1][chessboard->piece_count[piece_color][piece_type-1]]=i;
            chessboard->piece_count[piece_color][piece_type-1]++;

            //SET BIT
            if(piece_type==PAWN){
                SET_BIT_ONE(chessboard->pawns[piece_color],BOARD_120_To_64[i]);
                SET_BIT_ONE(chessboard->pawns[BOTH],BOARD_120_To_64[i]);
            }
        }
    }

}

//fill material score
void fill_materials(BOARD_STRUCT *  chessboard){
     int piece;
    int piece_type,player;
    int i;
    chessboard->materials[0]=0;
    chessboard->materials[1]=0;
    for(i=0;i<BOARD_LENGTH;i++){
        piece=chessboard->board[i];
        if(piece!=OUT_OF_BOARD && piece!=EMPTY){
            player=piece>>PIECE_ROTATE; //white or black
            piece_type=piece&PIECE_MASK;
            chessboard->materials[player]+=piece_values[piece_type-1];
        }
    }
}

//mirrors board vertically
//used for evaluation checking
//if mirrored and evalution score same then evaluation function is correct;
void mirror_board(BOARD_STRUCT* chessboard){
    int tmp_ep=NO_EP;
    int tmp_castle=0;
    int tmp_piece;
    int piece_type;
    int piece_color;
    int tmp_board[120];
    if(chessboard->castle_permissions&WHITE_KING_CASTLE){
        tmp_castle|=BLACK_KING_CASTLE;
    }
    if(chessboard->castle_permissions&WHITE_QUEEN_CASTLE){
        tmp_castle|=BLACK_QUEEN_CASTLE;
    }
      if(chessboard->castle_permissions&BLACK_KING_CASTLE){
        tmp_castle|=WHITE_KING_CASTLE;
    }
      if(chessboard->castle_permissions&BLACK_QUEEN_CASTLE){
        tmp_castle|=WHITE_QUEEN_CASTLE;
    }
    chessboard->castle_permissions=tmp_castle;
    if(chessboard->enPassant_square!=NO_EP){
        tmp_ep=mirror_120[chessboard->enPassant_square];
    }
    chessboard->enPassant_square=tmp_ep;
    for(int i=0;i<120;i++){

       tmp_board[i]=chessboard->board[mirror_120[i]];

    }
     for(int i=0;i<120;i++){
        tmp_piece=tmp_board[i];
        if(tmp_piece!=EMPTY && tmp_piece!=OUT_OF_BOARD){
            piece_color=tmp_piece>>PIECE_ROTATE;
            piece_type=tmp_piece&PIECE_MASK;
            tmp_piece=(((1-piece_color)<<PIECE_ROTATE)|piece_type);
        }
        chessboard->board[i]=tmp_piece;
    }
    chessboard->side^=1;
    fill_lists(chessboard);

    chessboard->position_key=generate_position_key(chessboard);
    fill_materials(chessboard);
    ASSERT(validate_board(chessboard));
}
