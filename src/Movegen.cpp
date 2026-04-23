#include "Movegen.h"
#include "Validate.h"

inline void add_quiet_move(BOARD_STRUCT* chessboard,UINT_32 mov,MOVELIST* movelist)
{
    //best killer
    if(chessboard->search_killer_moves[0][chessboard->ply]==mov){
        movelist->moves[movelist->counter].score=900000;
    //second killer
    }else if(chessboard->search_killer_moves[1][chessboard->ply]==mov){
         movelist->moves[movelist->counter].score=800000;
    }else{
         int piece=chessboard->board[FROM_ENCODE(mov)];
         movelist->moves[movelist->counter].score= chessboard->search_history[piece>>PIECE_ROTATE][piece&PIECE_MASK][TO_ENCODE(mov)];

    }
    movelist->moves[movelist->counter].id=mov;
    movelist->counter++;
}

inline void add_capture_move(BOARD_STRUCT*chessboard,UINT_32  mov,MOVELIST* movelist)
{
    movelist->moves[movelist->counter].id=mov;
    movelist->moves[movelist->counter].score=mvvlva[(chessboard->board[(FROM_ENCODE(mov))]&PIECE_MASK)-1][(CAPTURE(mov)&PIECE_MASK)-1]+1000000;//(CAPTURE(mov)&PIECE_MASK)-1
    movelist->counter++;
}

inline void add_ep_move(BOARD_STRUCT* chessboard,UINT_32  mov,MOVELIST* movelist)
{
    movelist->moves[movelist->counter].id=mov;
    movelist->moves[movelist->counter].score=105+1000000; //mvvlva score
    movelist->counter++;
}

inline void add_white_pawn_move_capture(BOARD_STRUCT*chessboard,const int from,const int to,const int capture,MOVELIST* movelist)
{
    ASSERT(is_piece_valid(capture));
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));

    if(from>=31 && from<=38)  //rank 7
    {
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,WHITE_QUEEN,0),movelist);
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,WHITE_ROOK,0),movelist);
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,WHITE_BISHOP,0),movelist);
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,WHITE_KNIGHT,0),movelist);
    }
    else
    {
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,EMPTY,0),movelist);
    }
}

inline void add_white_pawn_move(BOARD_STRUCT*chessboard,const int from,const int to,MOVELIST* movelist)
{

    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));

    if(from>=31 && from<=38)  //rank 7
    {
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,WHITE_QUEEN,0),movelist);
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,WHITE_ROOK,0),movelist);
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,WHITE_BISHOP,0),movelist);
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,WHITE_KNIGHT,0),movelist);
    }
    else
    {
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,EMPTY,0),movelist);
    }
}

inline void add_black_pawn_move_capture(BOARD_STRUCT*chessboard,const int from,const int to,const int capture,MOVELIST* movelist)
{
    ASSERT(is_piece_valid(capture));
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));

    if(from>=81 && from<=88)  //rank 2
    {
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,BLACK_QUEEN,0),movelist);
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,BLACK_ROOK,0),movelist);
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,BLACK_BISHOP,0),movelist);
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,BLACK_KNIGHT,0),movelist);
    }
    else
    {
        add_capture_move(chessboard,CREATE_MOVE(from,to,capture,EMPTY,0),movelist);
    }
}

inline void add_black_pawn_move(BOARD_STRUCT*chessboard,const int from,const int to,MOVELIST* movelist)
{
    ASSERT(is_square_on_board(from));
    ASSERT(is_square_on_board(to));

    if(from>=81 && from<=88)  //rank 2
    {
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,BLACK_QUEEN,0),movelist);
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,BLACK_ROOK,0),movelist);
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,BLACK_BISHOP,0),movelist);
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,BLACK_KNIGHT,0),movelist);
    }
    else
    {
        add_quiet_move(chessboard,CREATE_MOVE(from,to,EMPTY,EMPTY,0),movelist);
    }
}



//generate pseudo legal moves
void generate_all_moves(BOARD_STRUCT* chessboard,MOVELIST* movelist)
{

    ASSERT(validate_board(chessboard));
    movelist->counter=0;


    int side=chessboard->side;
    int square;
    int target_square;
    int target_piece;
    int direction;
    int i;
    int length;

    //pawn
    if(side==WHITE)
    {

        length=chessboard->piece_count[WHITE][PAWN_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[WHITE][PAWN_INDEX][i];
            ASSERT(is_square_on_board(square));

            //quiet move
            if(chessboard->board[square-10]==EMPTY)
            {
                add_white_pawn_move(chessboard,square,square-10,movelist);
                //pawn start 2 move up
                //rank 7
                if(square>=81 && square<=88 && chessboard->board[square-20]==EMPTY)
                {
                    add_quiet_move(chessboard,CREATE_MOVE(square,square-20,0,0,FLAG_PAWN_START),movelist);
                }
            }

            //capture
            target_square=square-9;
            target_piece=chessboard->board[target_square];
            if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==BLACK)
            {
                add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
            }
            //ep
            if(target_square==chessboard->enPassant_square)
            {
                add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
            }

            //capture
            target_square=square-11;
            target_piece=chessboard->board[target_square];
            if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==BLACK)
            {
                add_white_pawn_move_capture(chessboard,square, target_square,chessboard->board[target_square],movelist);
            }
            //ep
            if(target_square==chessboard->enPassant_square)
            {
                add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
            }
        }


    }
    else
    {

        length=chessboard->piece_count[BLACK][PAWN_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[BLACK][PAWN_INDEX][i];
            ASSERT(is_square_on_board(square));

            //quiet move
            if(chessboard->board[square+10]==EMPTY)
            {
                add_black_pawn_move(chessboard,square,square+10,movelist);
                //pawn start 2 move up
                //rank 2
                if(square>=31 && square<=38 && chessboard->board[square+20]==EMPTY)
                {
                    add_quiet_move(chessboard,CREATE_MOVE(square,square+20,0,0,FLAG_PAWN_START),movelist);
                }
            }


            //capture
            target_square=square+9;
            target_piece=chessboard->board[target_square];
            if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==WHITE)
            {
                add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
            }
            //ep
            if(target_square==chessboard->enPassant_square)
            {
                add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
            }

            //capture
            target_square=square+11;
            target_piece=chessboard->board[target_square];
            if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY &&(target_piece>>PIECE_ROTATE)==WHITE)
            {
                add_black_pawn_move_capture(chessboard,square, target_square,chessboard->board[target_square],movelist);
            }
            //ep
            if(target_square==chessboard->enPassant_square)
            {
                add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
            }
        }



    }





    //knight
    length=chessboard->piece_count[side][KNIGHT_INDEX];
    for(i=0; i<length; i++)
    {
        square=chessboard->piece_list[side][KNIGHT_INDEX][i];
        for(int j=0; j<8; j++)
        {
            target_square=square+knight_direction[j];
            target_piece=chessboard->board[target_square];
            if(target_piece==OUT_OF_BOARD)
            {
                continue;
            }
            if(target_piece==EMPTY)
            {
                add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
            }
            else if(target_piece>>PIECE_ROTATE!=side)
            {
                //if opponent piece
                add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
            }
        }
    }

    //rook
    length=chessboard->piece_count[side][ROOK_INDEX];
    for(i=0; i<length; i++)
    {
        square=chessboard->piece_list[side][ROOK_INDEX][i];
        for(int j=0; j<4; j++)
        {
            direction=rook_direction[j];
            target_square=square+direction;
            target_piece=chessboard->board[target_square];
            while(target_piece==EMPTY)
            {
                add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                target_square+=direction;
                target_piece=chessboard->board[target_square];
            }
            if(target_piece==OUT_OF_BOARD)
            {
                continue;
            }
            if(target_piece>>PIECE_ROTATE!=side)
            {
                add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
            }
        }
    }

    //bishop
    length=chessboard->piece_count[side][BISHOP_INDEX];
    for(i=0; i<length; i++)
    {
        square=chessboard->piece_list[side][BISHOP_INDEX][i];
        for(int j=0; j<4; j++)
        {
            direction=bishop_direction[j];
            target_square=square+direction;
            target_piece=chessboard->board[target_square];
            while(target_piece==EMPTY)
            {
                add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                target_square+=direction;
                target_piece=chessboard->board[target_square];
            }
            if(target_piece==OUT_OF_BOARD)
            {
                continue;
            }
            if(target_piece>>PIECE_ROTATE!=side)
            {
                add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
            }
        }
    }
    //queen
    length=chessboard->piece_count[side][QUEEN_INDEX];
    for(i=0; i<length; i++)
    {
        square=chessboard->piece_list[side][QUEEN_INDEX][i];
        for(int j=0; j<8; j++)
        {
            direction=queen_direction[j];
            target_square=square+direction;
            target_piece=chessboard->board[target_square];
            while(target_piece==EMPTY)
            {
                add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                target_square+=direction;
                target_piece=chessboard->board[target_square];
            }
            if(target_piece==OUT_OF_BOARD)
            {
                continue;
            }
            if(target_piece>>PIECE_ROTATE!=side)
            {
                add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
            }
        }
    }
    //king
    ASSERT(chessboard->piece_count[side][KING_INDEX]!=0);
    square=chessboard->piece_list[side][KING_INDEX][0];
    for(int i=0; i<8; i++)
    {
        target_square=square+king_direction[i];
        target_piece=chessboard->board[target_square];
        if(target_piece==OUT_OF_BOARD)
        {
            continue;
        }
        if(target_piece==EMPTY)
        {
            add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
        }
        else if(target_piece>>PIECE_ROTATE!=side)
        {
            //if opponent piece
            add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
        }
    }

    //castle
    int castle=chessboard->castle_permissions;

    if(side==WHITE)
    {


        if((castle&WHITE_KING_CASTLE)!=0)
        {
            if(chessboard->board[96]==EMPTY && chessboard->board[97]==EMPTY)
            {
                if(!is_square_attacked(96,1-side,chessboard)  && !is_square_attacked(95,1-side,chessboard) && !is_square_attacked(97,1-side,chessboard))
                {
                    add_quiet_move(chessboard,CREATE_MOVE(95,97,EMPTY,EMPTY,FLAG_CASTLE),movelist);
                }
            }
        }
        if((castle&WHITE_QUEEN_CASTLE)!=0)
        {
            if(chessboard->board[94]==EMPTY && chessboard->board[93]==EMPTY && chessboard->board[92]==EMPTY)
            {
                if(!is_square_attacked(94,1-side,chessboard) && !is_square_attacked(95,1-side,chessboard) && !is_square_attacked(93,1-side,chessboard))
                {
                    add_quiet_move(chessboard,CREATE_MOVE(95,93,EMPTY,EMPTY,FLAG_CASTLE),movelist);
                }
            }
        }


    }
    else
    {


        if((castle&BLACK_KING_CASTLE)!=0)
        {
            if(chessboard->board[26]==EMPTY && chessboard->board[27]==EMPTY)
            {
                if(!is_square_attacked(26,1-side,chessboard) &&!is_square_attacked(25,1-side,chessboard) && !is_square_attacked(27,1-side,chessboard))
                {
                    add_quiet_move(chessboard,CREATE_MOVE(25,27,EMPTY,EMPTY,FLAG_CASTLE),movelist);
                }
            }
        }
        if((castle&BLACK_QUEEN_CASTLE)!=0)
        {
            if(chessboard->board[24]==EMPTY && chessboard->board[23]==EMPTY && chessboard->board[22]==EMPTY)
            {
                if(!is_square_attacked(24,1-side,chessboard)&&!is_square_attacked(25,1-side,chessboard)  && !is_square_attacked(23,1-side,chessboard))
                {
                    add_quiet_move(chessboard,CREATE_MOVE(25,23,EMPTY,EMPTY,FLAG_CASTLE),movelist);
                }
            }
        }


    }

}

//generate legal moves
void generate_legal_moves(BOARD_STRUCT* chessboard,MOVELIST* movelist)
{
    int pin_ray[120]= {0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0
    };
    int king_move_ray[120];//= {0};
    int attackers[16];
    int king_square=chessboard->piece_list[chessboard->side][KING_INDEX][0];
    int square;
    int piece;
    int target_piece;
    int target_square;
    int attacker_square;
    int attacker_piece;
    int attacker_type;
    int is_attacker_sliding_piece=false;
    int side=chessboard->side;
    int pin_id;
    int piece_pin_number;
    int length;
    int direction;
    movelist->counter=0;
   // bool is_pinned=false;
    int i,j;
    for( i=0; i<8; i++)
    {
        king_move_ray[king_square+king_direction[i]]=0;
    }
    //king_move_ray[king_square]=9;
    int attacker_count=0;

    //**************************************************
    //calculate attackers

    //calculate sliding piece checks and pin
    calculate_pins(pin_ray,king_move_ray,attacker_count,attackers,chessboard->side,chessboard);

    //knight checks
    for( i=0; i<8; i++)
    {
        piece=chessboard->board[king_square+knight_direction[i]];
        if(piece==OUT_OF_BOARD || piece==EMPTY)
        {
            continue;
        }
        //if enemy knight
        if((piece>>PIECE_ROTATE)!=side && (piece&PIECE_MASK)==KNIGHT)
        {
            attackers[attacker_count]=king_square+knight_direction[i];
            attacker_count++;
        }
    }


    //pawn checks
    if(side==WHITE)
    {
        if(chessboard->board[king_square-11]==BLACK_PAWN )
        {
            attackers[attacker_count]=king_square-11;
            attacker_count++;
        }
        if(chessboard->board[king_square-9]== BLACK_PAWN )
        {
            attackers[attacker_count]=king_square-9;
            attacker_count++;
        }
    }
    else
    {
        if(chessboard->board[king_square+11]==WHITE_PAWN)  //up
        {
            attackers[attacker_count]=king_square+11;
            attacker_count++;
        }
        if(chessboard->board[king_square+9]==WHITE_PAWN)  //up
        {
            attackers[attacker_count]=king_square+9;
            attacker_count++;
        }
    }
    //**********************************************************

  // printf("attacker_count :%d\n",attacker_count);

    //if attacker count >=2 only legal move is kings move
    if(attacker_count==0)
    {

         //rook
        length=chessboard->piece_count[side][ROOK_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][ROOK_INDEX][i];
            for(j=0; j<4; j++)
            {
                direction=rook_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                piece_pin_number=pin_ray[square];

                while(target_piece==EMPTY)
                {
                    //if piece pinned
                    if(piece_pin_number!=0){
                            //only move in pin ray
                            if(pin_ray[target_square]==piece_pin_number){
                                 add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                            }
                    }else{
                         add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                    }

                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }

                //capture
                if(target_piece>>PIECE_ROTATE!=side)
                {
                    //if pinned can only capture same pin number
                    if(piece_pin_number!=0){
                        if(pin_ray[target_square]==piece_pin_number){
                            add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                        }
                    }else{
                        add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                    }
                }
            }
        }


        //bishop
        length=chessboard->piece_count[side][BISHOP_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][BISHOP_INDEX][i];
            for(j=0; j<4; j++)
            {
                direction=bishop_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                piece_pin_number=pin_ray[square];

                while(target_piece==EMPTY)
                {
                    //if piece pinned
                    if(piece_pin_number!=0){
                            //only move in pin ray
                            if(pin_ray[target_square]==piece_pin_number){
                                 add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                            }
                    }else{
                         add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                    }

                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }

                //capture
                if(target_piece>>PIECE_ROTATE!=side)
                {
                    //if pinned can only capture same pin number
                    if(piece_pin_number!=0){
                        if(pin_ray[target_square]==piece_pin_number){
                            add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                        }
                    }else{
                        add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                    }
                }
            }
        }

         //queen
        length=chessboard->piece_count[side][QUEEN_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][QUEEN_INDEX][i];
            for( j=0; j<8; j++)
            {
                direction=queen_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                piece_pin_number=pin_ray[square];

                while(target_piece==EMPTY)
                {
                    //if piece pinned
                    if(piece_pin_number!=0){
                            //only move in pin ray
                            if(pin_ray[target_square]==piece_pin_number){
                                 add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                            }
                    }else{
                         add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                    }

                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }

                //capture
                if(target_piece>>PIECE_ROTATE!=side)
                {
                    //if pinned can only capture same pin number
                    if(piece_pin_number!=0){
                        if(pin_ray[target_square]==piece_pin_number){
                            add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                        }
                    }else{
                        add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                    }
                }
            }
        }

          //knight
        length=chessboard->piece_count[side][KNIGHT_INDEX];

        for(i=0; i<length; i++)
        {

            square=chessboard->piece_list[side][KNIGHT_INDEX][i];

            //if piece is pinned
            if(pin_ray[square]!=0)
            {
                continue;
            }


            for( j=0; j<8; j++)
            {

                target_square=square+knight_direction[j];
                target_piece=chessboard->board[target_square];
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }
                if(target_piece==EMPTY)
                {
                    add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                }
                else if((target_piece>>PIECE_ROTATE)!=side)
                {
                    //if opponent piece
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }


        //pawn

        if(side==WHITE){

            length=chessboard->piece_count[WHITE][PAWN_INDEX];
            for(i=0; i<length; i++)
            {
                square=chessboard->piece_list[WHITE][PAWN_INDEX][i];
                ASSERT(is_square_on_board(square));

                //quiet move
                if(chessboard->board[square-10]==EMPTY)
                {
                    //if pinned

                    if(pin_ray[square]!=0){
                        if(pin_ray[square-10]==pin_ray[square]){
                             add_white_pawn_move(chessboard,square,square-10,movelist);
                        }
                    }else{
                         add_white_pawn_move(chessboard,square,square-10,movelist);
                    }

                    //pawn start 2 move up
                    //rank 7
                    if(square>=81 && square<=88 && chessboard->board[square-20]==EMPTY)
                    {
                        if(pin_ray[square]!=0){
                            if(pin_ray[square-20]==pin_ray[square]){
                                add_quiet_move(chessboard,CREATE_MOVE(square,square-20,0,0,FLAG_PAWN_START),movelist);
                            }
                        }else{
                            add_quiet_move(chessboard,CREATE_MOVE(square,square-20,0,0,FLAG_PAWN_START),movelist);
                        }
                    }
                }

                //capture
                target_square=square-9;
                target_piece=chessboard->board[target_square];
                if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==BLACK)
                {
                    if(pin_ray[square]!=0){
                        if(pin_ray[target_square]==pin_ray[square]){
                             add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                        }
                    }else{
                         add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                    }

                }
                //ep
                if(target_square==chessboard->enPassant_square)
                {
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }

                //capture
                target_square=square-11;
                target_piece=chessboard->board[target_square];
                if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==BLACK)
                {
                    if(pin_ray[square]!=0){
                        if(pin_ray[target_square]==pin_ray[square]){
                             add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                        }
                    }else{
                         add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                    }
                }

                //ep
                if(target_square==chessboard->enPassant_square)
                {
                   if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }
            }


        }else{

            length=chessboard->piece_count[BLACK][PAWN_INDEX];
            for(i=0; i<length; i++)
            {
                square=chessboard->piece_list[BLACK][PAWN_INDEX][i];
                ASSERT(is_square_on_board(square));

                //quiet move
                if(chessboard->board[square+10]==EMPTY)
                {
                    //if pinned

                    if(pin_ray[square]!=0){
                        if(pin_ray[square+10]==pin_ray[square]){
                             add_black_pawn_move(chessboard,square,square+10,movelist);
                        }
                    }else{
                         add_black_pawn_move(chessboard,square,square+10,movelist);
                    }

                    //pawn start 2 move up
                    //rank 2
                    if(square>=31 && square<=38 && chessboard->board[square+20]==EMPTY)
                    {
                        if(pin_ray[square]!=0){
                            if(pin_ray[square+20]==pin_ray[square]){
                                add_quiet_move(chessboard,CREATE_MOVE(square,square+20,0,0,FLAG_PAWN_START),movelist);
                            }
                        }else{
                            add_quiet_move(chessboard,CREATE_MOVE(square,square+20,0,0,FLAG_PAWN_START),movelist);
                        }
                    }
                }

                //capture
                target_square=square+9;
                target_piece=chessboard->board[target_square];
                if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==WHITE)
                {
                    if(pin_ray[square]!=0){
                        if(pin_ray[target_square]==pin_ray[square]){
                             add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                        }
                    }else{
                         add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                    }

                }
                //ep
                if(target_square==chessboard->enPassant_square)
                {
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }

                //capture
                target_square=square+11;
                target_piece=chessboard->board[target_square];
                if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==WHITE)
                {
                    if(pin_ray[square]!=0){
                        if(pin_ray[target_square]==pin_ray[square]){
                             add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                        }
                    }else{
                         add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                    }
                }

                //ep
                if(target_square==chessboard->enPassant_square)
                {
                   if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }
            }




        }


        //castle


         int castle=chessboard->castle_permissions;

        if(side==WHITE)
        {


            if((castle&WHITE_KING_CASTLE)!=0)
            {
                if(chessboard->board[96]==EMPTY && chessboard->board[97]==EMPTY)
                {
                    if(!is_square_attacked(96,1-side,chessboard) && !is_square_attacked(97,1-side,chessboard))
                    {
                        add_quiet_move(chessboard,CREATE_MOVE(95,97,EMPTY,EMPTY,FLAG_CASTLE),movelist);
                    }
                }
            }
            if((castle&WHITE_QUEEN_CASTLE)!=0)
            {
                if(chessboard->board[94]==EMPTY && chessboard->board[93]==EMPTY && chessboard->board[92]==EMPTY)
                {
                    if(!is_square_attacked(94,1-side,chessboard) && !is_square_attacked(93,1-side,chessboard))
                    {
                        add_quiet_move(chessboard,CREATE_MOVE(95,93,EMPTY,EMPTY,FLAG_CASTLE),movelist);
                    }
                }
            }


        }
        else
        {


            if((castle&BLACK_KING_CASTLE)!=0)
            {
                if(chessboard->board[26]==EMPTY && chessboard->board[27]==EMPTY)
                {
                    if(!is_square_attacked(26,1-side,chessboard)  && !is_square_attacked(27,1-side,chessboard))
                    {
                        add_quiet_move(chessboard,CREATE_MOVE(25,27,EMPTY,EMPTY,FLAG_CASTLE),movelist);
                    }
                }
            }
            if((castle&BLACK_QUEEN_CASTLE)!=0)
            {
                if(chessboard->board[24]==EMPTY && chessboard->board[23]==EMPTY && chessboard->board[22]==EMPTY)
                {
                    if(!is_square_attacked(24,1-side,chessboard)&& !is_square_attacked(23,1-side,chessboard))
                    {
                        add_quiet_move(chessboard,CREATE_MOVE(25,23,EMPTY,EMPTY,FLAG_CASTLE),movelist);
                    }
                }
            }


        }



    }
    else if(attacker_count==1)
    {
        //only one attacker
        attacker_square=attackers[0];
        pin_id=pin_ray[attacker_square];
        attacker_piece=chessboard->board[attacker_square];
        attacker_type=attacker_piece&PIECE_MASK;
        if(attacker_type==ROOK||attacker_type==QUEEN||attacker_type==BISHOP)
        {
            is_attacker_sliding_piece=true;
        }


        //knight
        length=chessboard->piece_count[side][KNIGHT_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][KNIGHT_INDEX][i];
            //if piece is pinned
            if(pin_ray[square]!=0)
            {
                continue;
            }


            for( j=0; j<8; j++)
            {

                target_square=square+knight_direction[j];
                target_piece=chessboard->board[target_square];
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }
                if(target_piece==EMPTY && pin_ray[target_square]==pin_id && is_attacker_sliding_piece)
                {
                    add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);
                }
                else if(target_square==attacker_square)
                {
                    //if opponent piece
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }

        //capture piece or block piece
        //rook
        length=chessboard->piece_count[side][ROOK_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][ROOK_INDEX][i];

            //if piece is blocking and opponent piece to threat king cant move to prevent different threat
            if(pin_ray[square]!=0 && pin_ray[square]!=pin_id)
            {
                continue;
            }
            for(int j=0; j<4; j++)
            {
                direction=rook_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                while(target_piece==EMPTY)
                {
                    if(is_attacker_sliding_piece && pin_ray[target_square]==pin_id)
                    {
                        //if target square is on ray

                        add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);


                    }
                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_square==attacker_square)
                {
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }

        //bishop
        length=chessboard->piece_count[side][BISHOP_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][BISHOP_INDEX][i];
            //if piece is blocking and opponent piece to threat king cant move to prevent different threat
            if(pin_ray[square]!=0 && pin_ray[square]!=pin_id)
            {
                continue;
            }

            for(int j=0; j<4; j++)
            {
                direction=bishop_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                while(target_piece==EMPTY)
                {
                    if(is_attacker_sliding_piece && pin_ray[target_square]==pin_id)
                    {
                        //if target square is on ray

                        add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);


                    }
                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_square==attacker_square)
                {
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }

        //queen
        length=chessboard->piece_count[side][QUEEN_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][QUEEN_INDEX][i];
            //if pinned pin id not zero
            if(pin_ray[square]!=0 && pin_ray[square]!=pin_id)
            {
                continue;
            }
            for(int j=0; j<8; j++)
            {
                direction=queen_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                while(target_piece==EMPTY)
                {
                    if(is_attacker_sliding_piece && pin_ray[target_square]==pin_id)
                    {
                        //if target square is on ray

                         add_quiet_move(chessboard,CREATE_MOVE(square,target_square,0,0,0),movelist);


                    }
                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_square==attacker_square)
                {
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }




        //pawn
        if(side==WHITE)
        {

            length=chessboard->piece_count[WHITE][PAWN_INDEX];
            for(i=0; i<length; i++)
            {

                square=chessboard->piece_list[WHITE][PAWN_INDEX][i];
                ASSERT(is_square_on_board(square));
                if(is_attacker_sliding_piece){
                    //quiet move
                    if(chessboard->board[square-10]==EMPTY )
                    {
                        if(pin_ray[square-10]==pin_id && pin_ray[square]==0){
                            add_white_pawn_move(chessboard,square,square-10,movelist);
                        }

                        //pawn start 2 move up
                        //rank 7
                        if(square>=81 && square<=88 && chessboard->board[square-20]==EMPTY)
                        {
                             if(pin_ray[square-20]==pin_id && pin_ray[square]==0){
                                   add_quiet_move(chessboard,CREATE_MOVE(square,square-20,0,0,FLAG_PAWN_START),movelist);
                             }
                        }
                    }
                }


                //capture
                target_square=square-9;
                target_piece=chessboard->board[target_square];
                if(target_square==attacker_square && pin_ray[square]==0)
                {
                    add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                }
                //ep
                /*
                if(target_square==chessboard->enPassant_square && (target_square+10)==attacker_square && attacker_type==PAWN)
                {
                    add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                }else if(target_square==chessboard->enPassant_square && is_attacker_sliding_piece && pin_ray[target_square]==pin_id){
                     add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                }
                */

                if(target_square==chessboard->enPassant_square){
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }
                //capture
                target_square=square-11;
                target_piece=chessboard->board[target_square];
                if(target_square==attacker_square && pin_ray[square]==0)
                {
                    add_white_pawn_move_capture(chessboard,square, target_square,chessboard->board[target_square],movelist);
                }
                //ep
                /*
                if(target_square==chessboard->enPassant_square && (target_square+10)==attacker_square && attacker_type==PAWN)
                {
                    add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                }else if(target_square==chessboard->enPassant_square && is_attacker_sliding_piece && pin_ray[target_square]==pin_id){
                     add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                }
                */
                  if(target_square==chessboard->enPassant_square){
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }
            }


        }
        else
        {

            length=chessboard->piece_count[BLACK][PAWN_INDEX];
            for(i=0; i<length; i++)
            {

                square=chessboard->piece_list[BLACK][PAWN_INDEX][i];
                ASSERT(is_square_on_board(square));
                if(is_attacker_sliding_piece){
                    //quiet move
                    if(chessboard->board[square+10]==EMPTY )
                    {
                        if(pin_ray[square+10]==pin_id && pin_ray[square]==0){
                            add_black_pawn_move(chessboard,square,square+10,movelist);
                        }

                        //pawn start 2 move up
                        //rank 2
                        if(square>=31 && square<=38 && chessboard->board[square+20]==EMPTY)
                        {
                             if(pin_ray[square+20]==pin_id && pin_ray[square]==0){
                                   add_quiet_move(chessboard,CREATE_MOVE(square,square+20,0,0,FLAG_PAWN_START),movelist);
                             }
                        }
                    }
                }


                //capture
                target_square=square+9;
                target_piece=chessboard->board[target_square];
                if(target_square==attacker_square && pin_ray[square]==0)
                {
                    add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                }
                //ep
                /*
                if(target_square==chessboard->enPassant_square && (target_square-10)==attacker_square && attacker_type==PAWN)
                {
                    add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                //if ep move block sliding piece
                }else if(target_square==chessboard->enPassant_square && is_attacker_sliding_piece && pin_ray[target_square]==pin_id){
                     add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                }
                */
                if(target_square==chessboard->enPassant_square){
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }
                //capture
                target_square=square+11;
                target_piece=chessboard->board[target_square];
                if(target_square==attacker_square && pin_ray[square]==0)
                {
                    add_black_pawn_move_capture(chessboard,square, target_square,chessboard->board[target_square],movelist);
                }
                //ep
               /*
                if(target_square==chessboard->enPassant_square && (target_square-10)==attacker_square && attacker_type==PAWN)
                {
                    add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                }else if(target_square==chessboard->enPassant_square && is_attacker_sliding_piece && pin_ray[target_square]==pin_id){
                     add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                }
               */
                 if(target_square==chessboard->enPassant_square){
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }
            }


        }

    }

    //king move
    for( i=0; i<8; i++)
    {
        square=king_square+king_direction[i];
        piece=chessboard->board[square];
        //if sliding piece checks king cant escape opposite direction
        if(king_move_ray[square]!=0)
        {
            continue;
        }
        if(piece==OUT_OF_BOARD)
        {
            continue;
        }
        if(is_square_attacked(square,side^1,chessboard))
        {
            continue;
        }

        if(piece==EMPTY)
        {
            add_quiet_move(chessboard,CREATE_MOVE(king_square,square,0,0,0),movelist);
        }
        else if(piece>>PIECE_ROTATE!=side)
        {
            add_capture_move(chessboard,CREATE_MOVE(king_square,square,piece,0,0),movelist);
        }
    }

}


inline void calculate_pins(int pin_ray[],int king_move_ray[],int& attacker_count,int attackers[],int side,BOARD_STRUCT* chessboard)
{

    int square;
    int piece;
    int piece_color;
    int piece_type;
    int friendly_piece_count;
    int enemy_piece_count;
    int ray_num=1;
    int direction;
    int king_square=chessboard->piece_list[side][KING_INDEX][0];
    int counter;

    //horizontal vertical
    for(int i=0; i<4; i++)
    {
        direction=rook_direction[i];
        square=king_square+direction;
        piece=chessboard->board[square];
        friendly_piece_count=0;
        enemy_piece_count=0;
        counter=0;

        while(piece!=OUT_OF_BOARD)
        {

            if(piece!=EMPTY)
            {
                piece_color=piece>>PIECE_ROTATE;
                piece_type=piece&PIECE_MASK;

                //enemy
                if(piece_color!=side)
                {

                    enemy_piece_count++;
                    //if enemy is rook or queen and no piece blocking the ray king is attacked
                    if(piece_type==ROOK || piece_type== QUEEN)
                    {
                        if(friendly_piece_count==0)
                        {
                            attackers[attacker_count]=square;
                            attacker_count++;
                            king_move_ray[king_square+rook_direction_mirrored[i]]=1;
                            if(chessboard->board[king_square+rook_direction[i]]==EMPTY){
                                 king_move_ray[king_square+rook_direction[i]]=1;
                            }
                            for(int j=1; j<=counter+1; j++)
                            {
                                pin_ray[king_square+j*rook_direction[i]]=ray_num;
                            }
                            ray_num++;
                            break;
                            //if one piece is in way ray still count ray
                        }
                        else if(friendly_piece_count==1)
                        {
                            for(int j=1; j<=counter+1; j++)
                            {
                                pin_ray[king_square+j*rook_direction[i]]=ray_num;
                            }
                            ray_num++;
                            break;
                        }
                        //if piece not rook or queen break
                    }
                    else
                    {
                        break;
                    }
                }
                //friend
                else
                {
                    friendly_piece_count++;
                    if(friendly_piece_count>1)
                    {
                        break;
                    }
                }
            }
            square+=direction;
            piece=chessboard->board[square];
            counter++;

        }
    }



    for(int i=0; i<4; i++)
    {
        direction=bishop_direction[i];
        square=king_square+direction;
        piece=chessboard->board[square];
        friendly_piece_count=0;
        enemy_piece_count=0;
        counter=0;

        while(piece!=OUT_OF_BOARD)
        {

            if(piece!=EMPTY)
            {
                piece_color=piece>>PIECE_ROTATE;
                piece_type=piece&PIECE_MASK;

                //enemy
                if(piece_color!=side)
                {

                    enemy_piece_count++;
                    //if enemy is bishop or queen and no piece blocking the ray king is attacked
                    if(piece_type==BISHOP || piece_type== QUEEN)
                    {
                        if(friendly_piece_count==0)
                        {
                            attackers[attacker_count]=square;
                            attacker_count++;
                            king_move_ray[king_square+bishop_direction_mirrored[i]]=1;
                            if(chessboard->board[king_square+bishop_direction[i]]==EMPTY){
                                 king_move_ray[king_square+bishop_direction[i]]=1;
                            }
                            for(int j=1; j<=counter+1; j++)
                            {
                                pin_ray[king_square+j*bishop_direction[i]]=ray_num;
                            }
                            ray_num++;
                            break;
                        }
                        else if(friendly_piece_count==1)
                        {
                            for(int j=1; j<=counter+1; j++)
                            {
                                pin_ray[king_square+j*bishop_direction[i]]=ray_num;
                            }
                            ray_num++;
                            break;
                        }
                        //if piece not rook or queen break
                    }
                    else
                    {
                        break;
                    }
                }
                //friend
                else
                {
                    friendly_piece_count++;
                    if(friendly_piece_count>1)
                    {
                        break;
                    }
                }
            }
            square+=direction;
            piece=chessboard->board[square];
            counter++;

        }
    }




}

bool is_move_exists(BOARD_STRUCT* chessboard,UINT_32 mov){
    MOVELIST movelist;
    generate_legal_moves(chessboard,&movelist);
    int i;
    int length=movelist.counter;
    for(i=0;i<length;i++){
        if(movelist.moves[i].id==mov){
            return true;
        }
    }
    return false;
}
bool is_move_exists_version_with_movelist(BOARD_STRUCT* chessboard,MOVELIST* movelist,UINT_32 mov){
    int i;
    int length=movelist->counter;
    for(i=0;i<length;i++){
        if(movelist->moves[i].id==mov){
            return true;
        }
    }
    return false;
}
























void generate_legal_capture_moves(BOARD_STRUCT* chessboard,MOVELIST* movelist)
{
    int pin_ray[120]= {0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0
    };
    int king_move_ray[120];//= {0};
    int attackers[16];
    int king_square=chessboard->piece_list[chessboard->side][KING_INDEX][0];
    int square;
    int piece;
    int target_piece;
    int target_square;
    int attacker_square;
    int attacker_piece;
    int attacker_type;
    int is_attacker_sliding_piece=false;
    int side=chessboard->side;
    int pin_id;
    int piece_pin_number;
    int length;
    int direction;
    movelist->counter=0;
   // bool is_pinned=false;
    int i,j;
    for( i=0; i<8; i++)
    {
        king_move_ray[king_square+king_direction[i]]=0;
    }
    //king_move_ray[king_square]=9;
    int attacker_count=0;

    //**************************************************
    //calculate attackers

    //calculate sliding piece checks and pin
    calculate_pins(pin_ray,king_move_ray,attacker_count,attackers,chessboard->side,chessboard);

    //knight checks
    for( i=0; i<8; i++)
    {
        piece=chessboard->board[king_square+knight_direction[i]];
        if(piece==OUT_OF_BOARD || piece==EMPTY)
        {
            continue;
        }
        //if enemy knight
        if((piece>>PIECE_ROTATE)!=side && (piece&PIECE_MASK)==KNIGHT)
        {
            attackers[attacker_count]=king_square+knight_direction[i];
            attacker_count++;
        }
    }


    //pawn checks
    if(side==WHITE)
    {
        if(chessboard->board[king_square-11]==BLACK_PAWN )
        {
            attackers[attacker_count]=king_square-11;
            attacker_count++;
        }
        if(chessboard->board[king_square-9]== BLACK_PAWN )
        {
            attackers[attacker_count]=king_square-9;
            attacker_count++;
        }
    }
    else
    {
        if(chessboard->board[king_square+11]==WHITE_PAWN)  //up
        {
            attackers[attacker_count]=king_square+11;
            attacker_count++;
        }
        if(chessboard->board[king_square+9]==WHITE_PAWN)  //up
        {
            attackers[attacker_count]=king_square+9;
            attacker_count++;
        }
    }
    //**********************************************************

  // printf("attacker_count :%d\n",attacker_count);

    //if attacker count >=2 only legal move is kings move
    if(attacker_count==0)
    {
        //only one


         //rook
        length=chessboard->piece_count[side][ROOK_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][ROOK_INDEX][i];
            for(j=0; j<4; j++)
            {
                direction=rook_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                piece_pin_number=pin_ray[square];

                while(target_piece==EMPTY)
                {
                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }

                //capture
                if(target_piece>>PIECE_ROTATE!=side)
                {
                    //if pinned can only capture same pin number
                    if(piece_pin_number!=0){
                        if(pin_ray[target_square]==piece_pin_number){
                            add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                        }
                    }else{
                        add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                    }
                }
            }
        }


        //bishop
        length=chessboard->piece_count[side][BISHOP_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][BISHOP_INDEX][i];
            for(j=0; j<4; j++)
            {
                direction=bishop_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                piece_pin_number=pin_ray[square];

                while(target_piece==EMPTY)
                {
                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }

                //capture
                if(target_piece>>PIECE_ROTATE!=side)
                {
                    //if pinned can only capture same pin number
                    if(piece_pin_number!=0){
                        if(pin_ray[target_square]==piece_pin_number){
                            add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                        }
                    }else{
                        add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                    }
                }
            }
        }

         //queen
        length=chessboard->piece_count[side][QUEEN_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][QUEEN_INDEX][i];
            for( j=0; j<8; j++)
            {
                direction=queen_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                piece_pin_number=pin_ray[square];

                while(target_piece==EMPTY)
                {

                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }

                //capture
                if(target_piece>>PIECE_ROTATE!=side)
                {
                    //if pinned can only capture same pin number
                    if(piece_pin_number!=0){
                        if(pin_ray[target_square]==piece_pin_number){
                            add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                        }
                    }else{
                        add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                    }
                }
            }
        }

          //knight
        length=chessboard->piece_count[side][KNIGHT_INDEX];

        for(i=0; i<length; i++)
        {

            square=chessboard->piece_list[side][KNIGHT_INDEX][i];

            //if piece is pinned
            if(pin_ray[square]!=0)
            {
                continue;
            }


            for( j=0; j<8; j++)
            {

                target_square=square+knight_direction[j];
                target_piece=chessboard->board[target_square];
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }

                if(target_piece!=EMPTY &&(target_piece>>PIECE_ROTATE)!=side)
                {
                    //if opponent piece
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }


        //pawn

        if(side==WHITE){

            length=chessboard->piece_count[WHITE][PAWN_INDEX];
            for(i=0; i<length; i++)
            {
                square=chessboard->piece_list[WHITE][PAWN_INDEX][i];
                ASSERT(is_square_on_board(square));

                //capture
                target_square=square-9;
                target_piece=chessboard->board[target_square];
                if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==BLACK)
                {
                    if(pin_ray[square]!=0){
                        if(pin_ray[target_square]==pin_ray[square]){
                             add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                        }
                    }else{
                         add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                    }

                }
                //ep
                if(target_square==chessboard->enPassant_square)
                {
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }

                //capture
                target_square=square-11;
                target_piece=chessboard->board[target_square];
                if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==BLACK)
                {
                    if(pin_ray[square]!=0){
                        if(pin_ray[target_square]==pin_ray[square]){
                             add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                        }
                    }else{
                         add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                    }
                }

                //ep
                if(target_square==chessboard->enPassant_square)
                {
                   if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }
            }


        }else{

            length=chessboard->piece_count[BLACK][PAWN_INDEX];
            for(i=0; i<length; i++)
            {
                square=chessboard->piece_list[BLACK][PAWN_INDEX][i];
                ASSERT(is_square_on_board(square));

                //capture
                target_square=square+9;
                target_piece=chessboard->board[target_square];
                if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==WHITE)
                {
                    if(pin_ray[square]!=0){
                        if(pin_ray[target_square]==pin_ray[square]){
                             add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                        }
                    }else{
                         add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                    }

                }
                //ep
                if(target_square==chessboard->enPassant_square)
                {
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }

                //capture
                target_square=square+11;
                target_piece=chessboard->board[target_square];
                if(target_piece!=OUT_OF_BOARD && target_piece!=EMPTY && (target_piece>>PIECE_ROTATE)==WHITE)
                {
                    if(pin_ray[square]!=0){
                        if(pin_ray[target_square]==pin_ray[square]){
                             add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                        }
                    }else{
                         add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                    }
                }

                //ep
                if(target_square==chessboard->enPassant_square)
                {
                   if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }
            }




        }


    }
    else if(attacker_count==1)
    {
        //only one attacker
        attacker_square=attackers[0];
        pin_id=pin_ray[attacker_square];
        attacker_piece=chessboard->board[attacker_square];
        attacker_type=attacker_piece&PIECE_MASK;
        if(attacker_type==ROOK||attacker_type==QUEEN||attacker_type==BISHOP)
        {
            is_attacker_sliding_piece=true;
        }


        //knight
        length=chessboard->piece_count[side][KNIGHT_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][KNIGHT_INDEX][i];
            //if piece is pinned
            if(pin_ray[square]!=0)
            {
                continue;
            }


            for( j=0; j<8; j++)
            {

                target_square=square+knight_direction[j];
                target_piece=chessboard->board[target_square];
                if(target_piece==OUT_OF_BOARD)
                {
                    continue;
                }
                if(target_piece!=EMPTY && target_square==attacker_square)
                {
                    //if opponent piece
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }

        //capture piece or block piece
        //rook
        length=chessboard->piece_count[side][ROOK_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][ROOK_INDEX][i];

            //if piece is blocking and opponent piece to threat king cant move to prevent different threat
            if(pin_ray[square]!=0 && pin_ray[square]!=pin_id)
            {
                continue;
            }
            for(int j=0; j<4; j++)
            {
                direction=rook_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                while(target_piece==EMPTY)
                {
                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_square==attacker_square)
                {
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }

        //bishop
        length=chessboard->piece_count[side][BISHOP_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][BISHOP_INDEX][i];
            //if piece is blocking and opponent piece to threat king cant move to prevent different threat
            if(pin_ray[square]!=0 && pin_ray[square]!=pin_id)
            {
                continue;
            }

            for(int j=0; j<4; j++)
            {
                direction=bishop_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                while(target_piece==EMPTY)
                {
                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_square==attacker_square)
                {
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }

        //queen
        length=chessboard->piece_count[side][QUEEN_INDEX];
        for(i=0; i<length; i++)
        {
            square=chessboard->piece_list[side][QUEEN_INDEX][i];
            //if pinned pin id not zero
            if(pin_ray[square]!=0 && pin_ray[square]!=pin_id)
            {
                continue;
            }
            for(int j=0; j<8; j++)
            {
                direction=queen_direction[j];
                target_square=square+direction;
                target_piece=chessboard->board[target_square];
                while(target_piece==EMPTY)
                {
                    target_square+=direction;
                    target_piece=chessboard->board[target_square];
                }
                if(target_square==attacker_square)
                {
                    add_capture_move(chessboard,CREATE_MOVE(square,target_square,target_piece,0,0),movelist);
                }
            }
        }




        //pawn
        if(side==WHITE)
        {

            length=chessboard->piece_count[WHITE][PAWN_INDEX];
            for(i=0; i<length; i++)
            {

                square=chessboard->piece_list[WHITE][PAWN_INDEX][i];
                ASSERT(is_square_on_board(square));

                //capture
                target_square=square-9;
                target_piece=chessboard->board[target_square];
                if(target_square==attacker_square && pin_ray[square]==0)
                {
                    add_white_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                }
                //ep

                 if(target_square==chessboard->enPassant_square){
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }

                //capture
                target_square=square-11;
                target_piece=chessboard->board[target_square];
                if(target_square==attacker_square && pin_ray[square]==0)
                {
                    add_white_pawn_move_capture(chessboard,square, target_square,chessboard->board[target_square],movelist);
                }
                //ep

                 if(target_square==chessboard->enPassant_square){
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }

            }


        }
        else
        {

            length=chessboard->piece_count[BLACK][PAWN_INDEX];
            for(i=0; i<length; i++)
            {

                square=chessboard->piece_list[BLACK][PAWN_INDEX][i];
                ASSERT(is_square_on_board(square));

                //capture
                target_square=square+9;
                target_piece=chessboard->board[target_square];
                if(target_square==attacker_square && pin_ray[square]==0)
                {
                    add_black_pawn_move_capture(chessboard,square,target_square,chessboard->board[target_square],movelist);
                }
                //ep

                 if(target_square==chessboard->enPassant_square){
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }

                //capture
                target_square=square+11;
                target_piece=chessboard->board[target_square];
                if(target_square==attacker_square && pin_ray[square]==0)
                {
                    add_black_pawn_move_capture(chessboard,square, target_square,chessboard->board[target_square],movelist);
                }
                //ep

                 if(target_square==chessboard->enPassant_square){
                    if(make_move(CREATE_MOVE(square,target_square,0,0,FLAG_EP),chessboard)){
                          add_ep_move(chessboard,CREATE_MOVE(square,target_square,0,0,FLAG_EP),movelist);
                          take_move(chessboard);
                    }
                }

            }


        }

    }

    //king move
    for( i=0; i<8; i++)
    {
        square=king_square+king_direction[i];
        piece=chessboard->board[square];
        //if sliding piece checks king cant escape opposite direction
        if(king_move_ray[square]!=0)
        {
            continue;
        }
        if(piece==OUT_OF_BOARD)
        {
            continue;
        }
        if(is_square_attacked(square,side^1,chessboard))
        {
            continue;
        }

        if(piece!=EMPTY &&piece>>PIECE_ROTATE!=side)
        {
            add_capture_move(chessboard,CREATE_MOVE(king_square,square,piece,0,0),movelist);
        }
    }

}

