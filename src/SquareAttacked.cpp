#include "SquareAttacked.h"
#include "Validate.h"
// return true if given square attacked by opponent color
bool is_square_attacked(int square, int attackerSide, BOARD_STRUCT* chessboard)
{
    int target_square;
    int piece;
    int piece_color;
    int piece_type;
    int i;
    int direction;
    //pawn


    ASSERT(is_side_valid(attackerSide));
    ASSERT(is_square_on_board(square));
    ASSERT(validate_board(chessboard));


    if(attackerSide==WHITE)
    {
        if(chessboard->board[square+11]==WHITE_PAWN || chessboard->board[square+9]==WHITE_PAWN) {
            return true;
        }
    }
    else
    {
        if(chessboard->board[square-11]==BLACK_PAWN  || chessboard->board[square-9]==BLACK_PAWN) {
            return true;
        }
    }

    //knight
    for(i=0; i<8; i++)
    {
        piece=chessboard->board[square+knight_direction[i]];
        if(piece!=OUT_OF_BOARD || piece!=EMPTY)
        {
            if((piece>>PIECE_ROTATE)==attackerSide && (piece&PIECE_MASK)==KNIGHT)
            {
                return true;
            }
        }
    }


    //rook queen
    for(i=0; i<4; i++)
    {
        direction=rook_direction[i];
        target_square=square+direction;
        piece=chessboard->board[target_square];
        while(piece!=OUT_OF_BOARD)
        {
            if(piece!=EMPTY)
            {
                piece_color=piece>>PIECE_ROTATE;
                piece_type=piece&PIECE_MASK;
                if(piece_color==attackerSide && (piece_type==ROOK || piece_type==QUEEN)){
                    return true;
                }
                break;
            }
            target_square+=direction;
            piece=chessboard->board[target_square];
        }
    }

    //bishop queen
    for(i=0; i<4; i++)
    {
         direction=bishop_direction[i];
        target_square=square+direction;
        piece=chessboard->board[target_square];
        while(piece!=OUT_OF_BOARD)
        {
            if(piece!=EMPTY)
            {
                piece_color=piece>>PIECE_ROTATE;
                piece_type=piece&PIECE_MASK;
                if(piece_color==attackerSide && (piece_type==BISHOP || piece_type==QUEEN)){
                    return true;
                }
                break;
            }
            target_square+=direction;
            piece=chessboard->board[target_square];
        }
    }

     //king
    for(int i=0;i<8;i++){
        piece=chessboard->board[square+king_direction[i]];
        if(piece!=OUT_OF_BOARD || piece!=EMPTY){
            if((piece>>PIECE_ROTATE)==attackerSide && (piece&PIECE_MASK)==KING){
                return true;
            }
        }
    }
    return false;
}


void print_attacked(int board[],int side,BOARD_STRUCT* c){
    printf("\n");
    for(int i=2;i<HEIGHT-2;i++){
        for(int j=1;j<WIDTH-1;j++){
            if(is_square_attacked(i*WIDTH+j,side,c)){
                printf("X ");
            }else{
                switch(board[i*WIDTH+j]){
                case WHITE_PAWN:
                        printf("P ");
                        break;
                case WHITE_ROOK:
                        printf("R ");
                        break;
                case WHITE_KNIGHT:
                        printf("N ");
                        break;
                case WHITE_BISHOP:
                        printf("B ");
                        break;
                case WHITE_QUEEN:
                        printf("Q ");
                        break;
                case WHITE_KING:
                        printf("K ");
                        break;
                 case BLACK_PAWN:
                        printf("p ");
                        break;
                case BLACK_ROOK:
                        printf("r ");
                        break;
                case BLACK_KNIGHT:
                        printf("n ");
                        break;
                case BLACK_BISHOP:
                        printf("b ");
                        break;
                case BLACK_QUEEN:
                        printf("q ");
                        break;
                case BLACK_KING:
                        printf("k ");
                        break;
                default:
                        printf(". ");
                        break;
               }
            }
        }
        printf("\n");
    }
}

