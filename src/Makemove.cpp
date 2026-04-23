#include "Makemove.h"
#include   "Validate.h"
#include "ChessBoard.h"
#include    "IO.h"


#define HASH_PIECE(square,piece_color,piece_type)   chessboard->position_key^=piece_keys[piece_color][piece_type][square]
#define HASH_EP(square) chessboard->position_key^=EP_key[square]
#define HASH_CASTLE(num) chessboard->position_key^=castle_key[num]
#define HASH_SIDE chessboard->position_key^=side_key


inline void clear_piece(const int square,BOARD_STRUCT* chessboard)
{

    ASSERT(is_square_on_board(square));

    int piece=chessboard->board[square];
    int piece_color=piece>>PIECE_ROTATE;
    int piece_type=(piece&PIECE_MASK)-1;

    ASSERT(is_piece_valid(piece));

    int piece_index=-1;
    int i=0;

    HASH_PIECE(square,piece_color,piece_type);

    //clear board
    ASSERT(chessboard->board[square]!=EMPTY);
    chessboard->board[square]=EMPTY;


    //update material score
    chessboard->materials[piece_color]-=piece_values[piece_type];

    //pawn bitboard
    if(piece_type==PAWN_INDEX){
        SET_BIT_ZERO(chessboard->pawns[piece_color],BOARD_120_To_64[square]);
        SET_BIT_ZERO(chessboard->pawns[BOTH],BOARD_120_To_64[square]);
    }


    //update piece_list
    int length=chessboard->piece_count[piece_color][piece_type];
    for(i=0; i<length; i++)
    {
        if(chessboard->piece_list[piece_color][piece_type][i]==square)
        {
            piece_index=i;
            break;
        }
    }

    ASSERT(piece_index!=-1);
    chessboard->piece_list[piece_color][piece_type][piece_index]=chessboard->piece_list[piece_color][piece_type][length-1];

    chessboard->piece_list[piece_color][piece_type][length-1]=EMPTY;
    chessboard->piece_count[piece_color][piece_type]--;
}

inline void add_piece(const int square,const int piece,BOARD_STRUCT* chessboard)
{
    ASSERT(is_square_on_board(square));
    ASSERT(is_piece_valid(piece));
    ASSERT(chessboard->board[square]==EMPTY);

    int piece_color=piece>>PIECE_ROTATE;
    int piece_type=(piece&PIECE_MASK)-1;

    //update position key
    HASH_PIECE(square,piece_color,piece_type);

    //add piece to the board
    chessboard->board[square]=piece;

    //update material
    chessboard->materials[piece_color]+=piece_values[piece_type];

    //pawn bitboard
    if(piece_type==PAWN_INDEX){
        SET_BIT_ONE(chessboard->pawns[piece_color],BOARD_120_To_64[square]);
        SET_BIT_ONE(chessboard->pawns[BOTH],BOARD_120_To_64[square]);
    }

    //update list
    chessboard->piece_list[piece_color][piece_type][chessboard->piece_count[piece_color][piece_type]] = square;
    chessboard->piece_count[piece_color][piece_type]++;

}

inline void move_piece(const int from,const int to,BOARD_STRUCT* chessboard)
{
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));
    ASSERT(chessboard->board[from]!=EMPTY);


    int i=0;
    int piece=chessboard->board[from];
    int piece_color=piece>>PIECE_ROTATE;
    int piece_type=(piece&PIECE_MASK)-1;

#ifdef DEBUG
    int piece_found=false;
#endif // DEBUG

    //move piece
    HASH_PIECE(from,piece_color,piece_type);
    chessboard->board[from]=EMPTY;

    HASH_PIECE(to,piece_color,piece_type);
    chessboard->board[to]=piece;

    //pawn bitboard
    //-1 defined so use index
    if(piece_type==PAWN_INDEX){
        SET_BIT_ZERO(chessboard->pawns[piece_color],BOARD_120_To_64[from]);
        SET_BIT_ZERO(chessboard->pawns[BOTH],BOARD_120_To_64[from]);
        SET_BIT_ONE(chessboard->pawns[piece_color],BOARD_120_To_64[to]);
        SET_BIT_ONE(chessboard->pawns[BOTH],BOARD_120_To_64[to]);
    }


    int length=chessboard->piece_count[piece_color][piece_type];
    for(i=0; i<length; i++)
    {
        if(chessboard->piece_list[piece_color][piece_type][i]==from)
        {
            chessboard->piece_list[piece_color][piece_type][i]=to;
#ifdef DEBUG
            piece_found=true;;
#endif // DEBUG
            break;


        }
    }
#ifdef DEBUG
    ASSERT(piece_found)
#endif // DEBUG
}


//make a move and update board
//return true if successfull
//make move and check if the move legal
bool make_move(UINT_32 mov,BOARD_STRUCT* chessboard)
{

    int from=FROM_ENCODE(mov);
    int to=TO_ENCODE(mov);
    int side=chessboard->side;

    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));
    ASSERT(is_piece_valid(chessboard->board[from]));
    //ASSERT(validate_board(chessboard));

   chessboard->history[chessboard->history_counter].position_key=chessboard->position_key;

    //if enpassant move
    if(mov& FLAG_EP){
        //remove pawn captured
        if(side==WHITE){
            clear_piece(to+10,chessboard);
        }else{

            clear_piece(to-10,chessboard);
        }
    }
    //castling move
    else if(mov&FLAG_CASTLE){
            if(to==97){
                 move_piece(98,96,chessboard);
            }else if(to==93){
                move_piece(91,94,chessboard);
            }else if(to==23){
                move_piece(21,24,chessboard);
            }else if(to==27){
                move_piece(28,26,chessboard);
            }
            #ifdef DEBUG
                else{
                    ASSERT(false);
                }
            #endif // DEBUG
    }



    if(chessboard->enPassant_square!=NO_EP){
        HASH_EP(chessboard->enPassant_square);
    }


   HASH_CASTLE(chessboard->castle_permissions);

      //save to history array
    int history_index=chessboard->history_counter;
    chessboard->history[history_index].move_id=mov;
    chessboard->history[history_index].castle_permissions=chessboard->castle_permissions;
    chessboard->history[history_index].enpassant_square=chessboard->enPassant_square;
    chessboard->history[history_index].fiftyMoveCounter=chessboard->fiftyMoveCounter;
    chessboard->history_counter++;

    chessboard->castle_permissions&=CASTLE_PERMISSION_ARRAY[from];
    chessboard->castle_permissions&=CASTLE_PERMISSION_ARRAY[to];
    chessboard->enPassant_square=NO_EP;


    HASH_CASTLE(chessboard->castle_permissions);

    //fiftymove rule

    chessboard->fiftyMoveCounter++;
    int captured=CAPTURE(mov);
    if(captured!=EMPTY){

        ASSERT(is_piece_valid(captured));
        clear_piece(to,chessboard);

        //if capture move reset counter
        chessboard->fiftyMoveCounter=0;
    }

    chessboard->ply++;


    if((chessboard->board[from]&PIECE_MASK)==PAWN){
        //pawn reset fifty move
        chessboard->fiftyMoveCounter=0;
        if(mov&FLAG_PAWN_START){
            if(side== WHITE){
                chessboard->enPassant_square=from-10;
                ASSERT(chessboard->enPassant_square>=71 && chessboard->enPassant_square<=78);
            }else{
                chessboard->enPassant_square=from+10;
                ASSERT(chessboard->enPassant_square>=41 && chessboard->enPassant_square<=48);
            }
            HASH_EP(chessboard->enPassant_square);
        }
    }

    //promote piece
    move_piece(from,to,chessboard);
    int promote_piece=PROMOTE(mov);
    if(promote_piece!=EMPTY){
        ASSERT(is_piece_valid(promote_piece));
        //clear pawn add promote piece
        clear_piece(to,chessboard);
        add_piece(to,promote_piece,chessboard);
    }

    chessboard->side^=1;
    HASH_SIDE;


    ASSERT(validate_board(chessboard));

    if(is_square_attacked(chessboard->piece_list[side][KING_INDEX][0],chessboard->side,chessboard)){
        take_move(chessboard);
        return false;
    }

    return true;

}

//use with generate legal moves
//no checks if move is legal
void make_move_legal(UINT_32 mov,BOARD_STRUCT* chessboard)
{

    int from=FROM_ENCODE(mov);
    int to=TO_ENCODE(mov);
    int side=chessboard->side;
    #ifdef DEBUG
    if(is_square_on_board(from)==false || is_square_on_board(to)==false){
        print_chessboard(chessboard->board);
        print_move(mov);
        print_lists(chessboard);
        printf("\n");
        take_move(chessboard);
        print_chessboard(chessboard->board);
        take_move(chessboard);
        print_chessboard(chessboard->board);
        take_move(chessboard);
        print_chessboard(chessboard->board);
        take_move(chessboard);
        print_chessboard(chessboard->board);
        take_move(chessboard);
        print_chessboard(chessboard->board);
        take_move(chessboard);
        print_chessboard(chessboard->board);
        take_move(chessboard);
        print_chessboard(chessboard->board);
    }
    #endif // DEBUG
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));
    ASSERT(is_piece_valid(chessboard->board[from]));
    //ASSERT(validate_board(chessboard));

   chessboard->history[chessboard->history_counter].position_key=chessboard->position_key;

    //if enpassant move
    if(mov& FLAG_EP){
        //remove pawn captured
        if(side==WHITE){
            clear_piece(to+10,chessboard);
        }else{

            clear_piece(to-10,chessboard);
        }
    }
    //castling move
    else if(mov&FLAG_CASTLE){
            if(to==97){
                 move_piece(98,96,chessboard);
            }else if(to==93){
                move_piece(91,94,chessboard);
            }else if(to==23){
                move_piece(21,24,chessboard);
            }else if(to==27){
                move_piece(28,26,chessboard);
            }
            #ifdef DEBUG
                else{
                    ASSERT(false);
                }
            #endif // DEBUG
    }



    if(chessboard->enPassant_square!=NO_EP){
        HASH_EP(chessboard->enPassant_square);
    }


   HASH_CASTLE(chessboard->castle_permissions);

      //save to history array
    int history_index=chessboard->history_counter;
    chessboard->history[history_index].move_id=mov;
    chessboard->history[history_index].castle_permissions=chessboard->castle_permissions;
    chessboard->history[history_index].enpassant_square=chessboard->enPassant_square;
    chessboard->history[history_index].fiftyMoveCounter=chessboard->fiftyMoveCounter;
    chessboard->history_counter++;

    chessboard->castle_permissions&=CASTLE_PERMISSION_ARRAY[from];
    chessboard->castle_permissions&=CASTLE_PERMISSION_ARRAY[to];
    chessboard->enPassant_square=NO_EP;


    HASH_CASTLE(chessboard->castle_permissions);

    //fiftymove rule

    chessboard->fiftyMoveCounter++;
    int captured=CAPTURE(mov);
    if(captured!=EMPTY){

        ASSERT(is_piece_valid(captured));
        clear_piece(to,chessboard);

        //if capture move reset counter
        chessboard->fiftyMoveCounter=0;
    }

    chessboard->ply++;


    if((chessboard->board[from]&PIECE_MASK)==PAWN){
        //pawn move resets fifty move
        chessboard->fiftyMoveCounter=0;
        if(mov&FLAG_PAWN_START){
            if(side== WHITE){
                chessboard->enPassant_square=from-10;
                ASSERT(chessboard->enPassant_square>=71 && chessboard->enPassant_square<=78);
            }else{
                chessboard->enPassant_square=from+10;
                ASSERT(chessboard->enPassant_square>=41 && chessboard->enPassant_square<=48);
            }
            HASH_EP(chessboard->enPassant_square);
        }
    }

    //promote piece
    move_piece(from,to,chessboard);
    int promote_piece=PROMOTE(mov);
    if(promote_piece!=EMPTY){
        ASSERT(is_piece_valid(promote_piece));
        //clear pawn add promote piece
        clear_piece(to,chessboard);
        add_piece(to,promote_piece,chessboard);
    }

    chessboard->side^=1;
    HASH_SIDE;


    ASSERT(validate_board(chessboard));



}


void take_move(BOARD_STRUCT* chessboard){

     ASSERT(validate_board(chessboard));

     chessboard->history_counter--;
     chessboard->ply--;
     int history_counter=chessboard->history_counter;

     UINT_32 mov=chessboard->history[history_counter].move_id;
     int from=FROM_ENCODE(mov);
     int to=TO_ENCODE(mov);

     ASSERT(is_square_on_board(from));
     ASSERT(is_square_on_board(to));




     chessboard->castle_permissions=chessboard->history[history_counter].castle_permissions;
     chessboard->fiftyMoveCounter=chessboard->history[history_counter].fiftyMoveCounter;
     chessboard->enPassant_square=chessboard->history[history_counter].enpassant_square;


     chessboard->side^=1;



     //en passant takeback
     if(mov&FLAG_EP){
        if(chessboard->side==WHITE){
            add_piece(to+10,BLACK_PAWN,chessboard);
        }else{
            add_piece(to-10,WHITE_PAWN,chessboard);
        }
     }

     //castle takeback
     else if(mov& FLAG_CASTLE){
            if(to==97){
                 move_piece(96,98,chessboard);
            }else if(to==93){
                move_piece(94,91,chessboard);
            }else if(to==23){
                move_piece(24,21,chessboard);
            }else if(to==27){
                move_piece(26,28,chessboard);
            }
     }
     move_piece(to,from,chessboard);

     int captured=CAPTURE(mov);
     if(captured!=EMPTY){
        ASSERT(is_piece_valid(captured));
        add_piece(to,captured,chessboard);
     }

     if(PROMOTE(mov)!=EMPTY){
         ASSERT(is_piece_valid(PROMOTE(mov)) &&    ((PROMOTE(mov)&PIECE_MASK) != PAWN));
         clear_piece(from,chessboard);
         if(chessboard->side==WHITE){
             add_piece(from,WHITE_PAWN,chessboard);
         }else{
             add_piece(from,BLACK_PAWN,chessboard);
         }
     }

      chessboard->position_key=chessboard->history[history_counter].position_key;
     ASSERT(validate_board(chessboard));
}


//used for null move pruning
void make_null_move(BOARD_STRUCT* chessboard){
    ASSERT(validate_board(chessboard));
    chessboard->ply++;
    chessboard->history[chessboard->history_counter].position_key=chessboard->position_key;

    if(chessboard->enPassant_square!=NO_EP){
        HASH_EP(chessboard->enPassant_square);
    }

     int history_index=chessboard->history_counter;
    chessboard->history[history_index].move_id=NO_MOVE;
    chessboard->history[history_index].castle_permissions=chessboard->castle_permissions;
    chessboard->history[history_index].enpassant_square=chessboard->enPassant_square;
    chessboard->history[history_index].fiftyMoveCounter=chessboard->fiftyMoveCounter;
    chessboard->enPassant_square=NO_EP;
    chessboard->side^=1;
    chessboard->history_counter++;

    HASH_SIDE;
    ASSERT(validate_board(chessboard));
}

//used for null move pruning
void take_null_move(BOARD_STRUCT* chessboard){
    ASSERT(validate_board(chessboard));
    chessboard->history_counter--;
    chessboard->ply--;

     int history_counter=chessboard->history_counter;
     chessboard->castle_permissions=chessboard->history[history_counter].castle_permissions;
     chessboard->fiftyMoveCounter=chessboard->history[history_counter].fiftyMoveCounter;
     chessboard->enPassant_square=chessboard->history[history_counter].enpassant_square;

     chessboard->side^=1;
     chessboard->position_key=chessboard->history[history_counter].position_key;


    ASSERT(validate_board(chessboard));
}
