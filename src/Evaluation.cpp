#include "Evaluation.h"

int evaluation(const BOARD_STRUCT* chessboard)
{
    //material score
    int score=chessboard->materials[WHITE]-chessboard->materials[BLACK];


    int i;
    int square;
    int square_64;
    bool passed_pawn;
    UINT_64 bishop_friendly_pawn_intersect;
    int bishop_intersect_count;




    if(!chessboard->piece_count[WHITE][PAWN_INDEX]  &&  !chessboard->piece_count[BLACK][PAWN_INDEX] &&is_material_draw(chessboard)){
        return 0;
    }

   UINT_64 piece_bitboard[2][6]={{0,0,0,0,0,0},{0,0,0,0,0,0}};


    //pawn
    //white
    int len=chessboard->piece_count[WHITE][PAWN_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[WHITE][PAWN_INDEX][i];
        square_64=BOARD_120_To_64[square];
        score+=pawn_score_table[square_64];
        passed_pawn=false;
        //masks
        //pawn isolated penalty
        if((isolated_pawn_mask[square_64]&chessboard->pawns[WHITE])==0){
            score+=PAWN_ISOLATED_SCORE;
        }

        //no enemy blocking score up
        //passed pawn
        if((pass_pawn_white_bitboard_mask[square_64]&chessboard->pawns[BLACK])==0){
            score+=pawn_passed_score[7-(square_64/8)];
            passed_pawn=true;
        }

        //doubled pawn
        //if friendly pawn blocking pawn
        if((white_doubled_pawn_bitboard_mask[square_64]&chessboard->pawns[WHITE])!=0 ){
            //if no friendly pawn defending advanced pawn
            if((left_right_bitboard_mask[square_64]&chessboard->pawns[WHITE])==0){
                score+=DOUBLED_PAWN_SCORE;
            }
        }

        //connected pawn
        if((diagonal_mask[square_64]&chessboard->pawns[WHITE])!=0){
            score+=CONNECTED_PAWN_BASE_SCORE;
            //if pawn is a passed pawn
            if(passed_pawn){
                //if left
                //if conected pawn is also passed pawn
                if((left_diagonal_mask[square_64]&chessboard->pawns[WHITE])!=0){
                    if((pass_pawn_white_bitboard_mask[square_64-1]&chessboard->pawns[BLACK])==0){
                         score+=connected_pawn_passed_score[7-(square_64/8)];
                    }
                }else{
                    if((pass_pawn_white_bitboard_mask[square_64+1]&chessboard->pawns[BLACK])==0){
                         score+=connected_pawn_passed_score[7-(square_64/8)];
                    }
                }
            }
        }

    }

    //black
    len=chessboard->piece_count[BLACK][PAWN_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[BLACK][PAWN_INDEX][i];
        square_64=BOARD_120_To_64[square];
        score-=pawn_score_table[mirror_64[square_64]];


          passed_pawn=false;
         //masks
        //pawn isolated penalty
        if((isolated_pawn_mask[square_64]&chessboard->pawns[BLACK])==0){
            score-=PAWN_ISOLATED_SCORE;
        }

        //no enemy blocking score up
        //passed pawn
        if((pass_pawn_black_bitboard_mask[square_64]&chessboard->pawns[WHITE])==0){
            score-=pawn_passed_score[(square_64/8)];
            passed_pawn=true;
        }


        //doubled pawn
        //if friendly pawn blocking pawn
        if((black_doubled_pawn_bitboard_mask[square_64]&chessboard->pawns[BLACK])!=0 ){
            //if no friendly pawn defending advanced pawn
            if((left_right_bitboard_mask[square_64]&chessboard->pawns[BLACK])==0){
                score-=DOUBLED_PAWN_SCORE;
            }
        }


          //connected pawn
        if((diagonal_mask[square_64]&chessboard->pawns[BLACK])!=0){
            score-=CONNECTED_PAWN_BASE_SCORE;
            //if pawn is a passed pawn
            if(passed_pawn){
                //if left
                //if conected pawn is also passed pawn
                if((left_diagonal_mask[square_64]&chessboard->pawns[BLACK])!=0){
                    if((pass_pawn_black_bitboard_mask[square_64-1]&chessboard->pawns[WHITE])==0){
                         score-=connected_pawn_passed_score[(square_64/8)];
                    }
                }else{
                    if((pass_pawn_black_bitboard_mask[square_64+1]&chessboard->pawns[WHITE])==0){
                         score-=connected_pawn_passed_score[(square_64/8)];
                    }
                }
            }
        }

    }


    piece_bitboard[WHITE][PAWN_INDEX]=chessboard->pawns[WHITE];
    piece_bitboard[BLACK][PAWN_INDEX]=chessboard->pawns[BLACK];


    //knight
    len=chessboard->piece_count[WHITE][KNIGHT_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[WHITE][KNIGHT_INDEX][i];
        square_64=BOARD_120_To_64[square];
        SET_BIT_ONE(piece_bitboard[WHITE][KNIGHT_INDEX],square_64);
        score+=knight_score_table[square_64];
    }

    len=chessboard->piece_count[BLACK][KNIGHT_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[BLACK][KNIGHT_INDEX][i];
        square_64=BOARD_120_To_64[square];
        SET_BIT_ONE(piece_bitboard[BLACK][KNIGHT_INDEX],square_64);
        score-=knight_score_table[mirror_64[square_64]];
    }



    //bishop
    len=chessboard->piece_count[WHITE][BISHOP_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[WHITE][BISHOP_INDEX][i];
        square_64=BOARD_120_To_64[square];
        score+=bishop_score_table[square_64];
        SET_BIT_ONE(piece_bitboard[WHITE][BISHOP_INDEX],square_64);
        bishop_friendly_pawn_intersect=bishop_attack_bitboard_mask[square_64]&chessboard->pawns[WHITE];

        //if  friendly pawn blocking bishop
        if(bishop_friendly_pawn_intersect){

            bishop_intersect_count=count_bits(bishop_friendly_pawn_intersect);
            score+=BAD_BISHOP_SCORE*(bishop_intersect_count);
        }

    }
    len=chessboard->piece_count[BLACK][BISHOP_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[BLACK][BISHOP_INDEX][i];
        square_64=BOARD_120_To_64[square];
        score-=bishop_score_table[mirror_64[square_64]];
        SET_BIT_ONE(piece_bitboard[BLACK][BISHOP_INDEX],square_64);

        bishop_friendly_pawn_intersect=bishop_attack_bitboard_mask[square_64]&chessboard->pawns[BLACK];

        //if friendly pawn blocking bishop
        if(bishop_friendly_pawn_intersect){
            bishop_intersect_count=count_bits(bishop_friendly_pawn_intersect);
            score-=BAD_BISHOP_SCORE*(bishop_intersect_count);
        }

    }



    //rook
    len=chessboard->piece_count[WHITE][ROOK_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[WHITE][ROOK_INDEX][i];
        square_64=BOARD_120_To_64[square];
        score+=rook_score_table[square_64];

        SET_BIT_ONE(piece_bitboard[WHITE][ROOK_INDEX],square_64);


         //no pawn on the way vertical
        if(!(chessboard->pawns[BOTH]&vertical_bitboard_mask[square_64%8])){
            score +=ROOK_OPEN_FILE;
            //not friend pawn in the way vertical
        }else if(!(chessboard->pawns[WHITE]&vertical_bitboard_mask[square_64%8])){
            score+=ROOK_SEMI_OPEN_FILE;
        }
    }

    len=chessboard->piece_count[BLACK][ROOK_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[BLACK][ROOK_INDEX][i];

        square_64=BOARD_120_To_64[square];
        score-=rook_score_table[mirror_64[square_64]];
        SET_BIT_ONE(piece_bitboard[BLACK][ROOK_INDEX],square_64);

        if(!(chessboard->pawns[BOTH]&vertical_bitboard_mask[square_64%8])){
            score -=ROOK_OPEN_FILE;
        }else if(!(chessboard->pawns[BLACK]&vertical_bitboard_mask[square_64%8])){
            score-=ROOK_SEMI_OPEN_FILE;
        }
    }



    //queen
    len=chessboard->piece_count[WHITE][QUEEN_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[WHITE][QUEEN_INDEX][i];

        square_64=BOARD_120_To_64[square];
        SET_BIT_ONE(piece_bitboard[WHITE][QUEEN_INDEX],square_64);
         //no pawn on the way vertical
        if(!(chessboard->pawns[BOTH]&vertical_bitboard_mask[square_64%8])){
            score +=QUEEN_OPEN_FILE;
            //not friend pawn in the way vertical
        }else if(!(chessboard->pawns[WHITE]&vertical_bitboard_mask[square_64%8])){
            score+=QUEEN_SEMI_OPEN_FILE;
        }
    }
    len=chessboard->piece_count[BLACK][QUEEN_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[BLACK][QUEEN_INDEX][i];
        square_64=BOARD_120_To_64[square];
        SET_BIT_ONE(piece_bitboard[BLACK][QUEEN_INDEX],square_64);



        if(!(chessboard->pawns[BOTH]&vertical_bitboard_mask[square_64%8])){
            score -=QUEEN_OPEN_FILE;
        }else if(!(chessboard->pawns[BLACK]&vertical_bitboard_mask[square_64%8])){
            score-=QUEEN_SEMI_OPEN_FILE;
        }
    }



    //king

        square=chessboard->piece_list[WHITE][KING_INDEX][0];
        square_64=BOARD_120_To_64[square];
        SET_BIT_ONE(piece_bitboard[WHITE][KING_INDEX],square_64);

        if(chessboard->materials[BLACK]<=END_GAME_MATERIAL){
            score+=KingE[mirror_64[square_64]];
        }else{
            score+=KingO[mirror_64[square_64]];
            //pawn shield
            score+=count_bits(king_inner_ring[square_64]&piece_bitboard[WHITE][PAWN_INDEX])*PAWN_SHIELD_SCORE;
        }
        //score+=king_score_table[WHITE][square];


        square=chessboard->piece_list[BLACK][KING_INDEX][0];
        square_64=BOARD_120_To_64[square];
        SET_BIT_ONE(piece_bitboard[BLACK][KING_INDEX],square_64);
        if(chessboard->materials[WHITE]<=END_GAME_MATERIAL){
            score-=KingE[square_64];
        }else{
            score-=KingO[square_64];
            //pawn shield
            score-=count_bits(king_inner_ring[square_64]&piece_bitboard[BLACK][PAWN_INDEX])*PAWN_SHIELD_SCORE;
        }

       // score-=king_score_table[BLACK][square];


    //**************************************************


    UINT_64 all_pieces[2];
    all_pieces[WHITE]=piece_bitboard[WHITE][PAWN_INDEX]|piece_bitboard[WHITE][ROOK_INDEX]|piece_bitboard[WHITE][KNIGHT_INDEX]|
                                         piece_bitboard[WHITE][BISHOP_INDEX]|piece_bitboard[WHITE][QUEEN_INDEX]|piece_bitboard[WHITE][KING_INDEX];
    all_pieces[BLACK]=piece_bitboard[BLACK][PAWN_INDEX]|piece_bitboard[BLACK][ROOK_INDEX]|piece_bitboard[BLACK][KNIGHT_INDEX]|
                                         piece_bitboard[BLACK][BISHOP_INDEX]|piece_bitboard[BLACK][QUEEN_INDEX]|piece_bitboard[BLACK][KING_INDEX];
    /*
    printf("piece bitboards\n");
    for(int i;i<2;i++){
        for(int j=0;j<6;j++){
            printf("\n");
            print_bitboard(piece_bitboard[i][j]);
        }
    }
    */
    //printf("attack bitboards\n");
    UINT_64 attack_tables[2][6]={{0,0,0,0,0,0},{0,0,0,0,0,0}};
    UINT_64 defend_tables[2][6]={{0,0,0,0,0,0},{0,0,0,0,0,0}};
    create_attack_tables(chessboard,piece_bitboard,attack_tables,all_pieces,defend_tables);
    /*
    UINT_64 all_attack_table[2];
    all_attack_table[WHITE]=attack_tables[WHITE][PAWN_INDEX]|attack_tables[WHITE][ROOK_INDEX]|attack_tables[WHITE][KNIGHT_INDEX]|
    attack_tables[WHITE][BISHOP_INDEX]|attack_tables[WHITE][QUEEN_INDEX]|attack_tables[WHITE][KING_INDEX];

    all_attack_table[BLACK]=attack_tables[BLACK][PAWN_INDEX]|attack_tables[BLACK][ROOK_INDEX]|attack_tables[BLACK][KNIGHT_INDEX]|
    attack_tables[BLACK][BISHOP_INDEX]|attack_tables[BLACK][QUEEN_INDEX]|attack_tables[BLACK][KING_INDEX];
    */
     UINT_64 all_defend_table[2];
    all_defend_table[WHITE]=defend_tables[WHITE][PAWN_INDEX]|defend_tables[WHITE][ROOK_INDEX]|defend_tables[WHITE][KNIGHT_INDEX]|
    defend_tables[WHITE][BISHOP_INDEX]|defend_tables[WHITE][QUEEN_INDEX];

    all_defend_table[BLACK]=defend_tables[BLACK][PAWN_INDEX]|defend_tables[BLACK][ROOK_INDEX]|defend_tables[BLACK][KNIGHT_INDEX]|
    defend_tables[BLACK][BISHOP_INDEX]|defend_tables[BLACK][QUEEN_INDEX];
    /*
     for(int i=0;i<2;i++){
        for(int j=0;j<6;j++){
            printf("\n");
            print_bitboard(attack_tables[i][j]);
        }
    }
    */
    // double bishop
    if(chessboard->piece_count[WHITE][BISHOP_INDEX]>=2){
        score+=BISHOP_PAIR_SCORE;
    }if(chessboard->piece_count[BLACK][BISHOP_INDEX]>=2){
        score-=BISHOP_PAIR_SCORE;
    }

    int counter;

    //mobility
    for(i=0;i<6;i++){
        counter=count_bits(attack_tables[WHITE][i]);
        score+=counter*mobility_multiplier[i];
    }
    for(i=0;i<6;i++){
        counter=count_bits(attack_tables[BLACK][i]);
        score-=counter*mobility_multiplier[i];
    }


    //defend other pieces
    //not include king
    for(i=0;i<5;i++){
        counter=count_bits((defend_tables[WHITE][i]&all_pieces[WHITE]));
        score+=counter*defend_multiplier[i];
    }
    for(i=0;i<5;i++){
        counter=count_bits((defend_tables[BLACK][i]&all_pieces[BLACK]));
        score-=counter*defend_multiplier[i];
    }

    //center control
    //pawn is worth more
    score+=count_bits(piece_bitboard[WHITE][PAWN_INDEX]&CENTER_BITBOARD)*CENTER_CONTROL_PAWN_SCORE;
    score-=count_bits(piece_bitboard[BLACK][PAWN_INDEX]&CENTER_BITBOARD)*CENTER_CONTROL_PAWN_SCORE;
    score+=count_bits(all_pieces[WHITE]&(~piece_bitboard[WHITE][PAWN_INDEX])&CENTER_BITBOARD)*CENTER_CONTROL_PIECES_SCORE;
    score-=count_bits(all_pieces[BLACK]&(~piece_bitboard[BLACK][PAWN_INDEX])&CENTER_BITBOARD)*CENTER_CONTROL_PIECES_SCORE;


    //king safety
    //attack king zone
    //WHITE
    square=chessboard->piece_list[BLACK][KING_INDEX][0];
    square_64=BOARD_120_To_64[square];

    int king_index_counter=0;
    for(i=0;i<6;i++){
        //squares that 2 square near king and white attack and not defended by black
        king_index_counter+=count_bits((attack_tables[WHITE][i]&king_attack_zone[square_64])&(~all_defend_table[BLACK]))*king_attack_index[i];
    }
    if(king_index_counter>99){
        king_index_counter=99;
    }
   // printf("king safety w %d\n",king_index_counter);

    score+=SafetyTable[king_index_counter]/2;

    //BLACK
    square=chessboard->piece_list[WHITE][KING_INDEX][0];
    square_64=BOARD_120_To_64[square];
    king_index_counter=0;
    for(i=0;i<6;i++){
        king_index_counter+=count_bits(attack_tables[BLACK][i]&king_attack_zone[square_64]&(~all_defend_table[WHITE]))*king_attack_index[i];
    }
    if(king_index_counter>99){
        king_index_counter=99;
    }
   // printf("king safety b %d\n",king_index_counter);
    score-=SafetyTable[king_index_counter]/2;


    score+=count_bits(king_attack_zone[BOARD_120_To_64[chessboard->piece_list[WHITE][KING_INDEX][0]]]&all_pieces[WHITE])*NEARBY_PIECES_BONUS;
    score-=count_bits(king_attack_zone[BOARD_120_To_64[chessboard->piece_list[BLACK][KING_INDEX][0]]]&all_pieces[BLACK])*NEARBY_PIECES_BONUS;

    //if turn is black negative score is used for negamax
    if(chessboard->side==BLACK){
            return -score;
    }
    return score;
}

bool is_material_draw(const BOARD_STRUCT* chessboard){
    int difference;
    if((!chessboard->piece_count[WHITE][ROOK_INDEX] )&&
        (!chessboard->piece_count[BLACK][ROOK_INDEX] ) &&
       (!chessboard->piece_count[WHITE][QUEEN_INDEX] ) &&
       (!chessboard->piece_count[BLACK][QUEEN_INDEX] )){
           //if every piece


           if((!chessboard->piece_count[WHITE][BISHOP_INDEX] )  &&  (!chessboard->piece_count[BLACK][BISHOP_INDEX] ) ){


                if((chessboard->piece_count[WHITE][KNIGHT_INDEX] <3) && (chessboard->piece_count[BLACK][KNIGHT_INDEX] <3) ){
                    return true;
                }

           }else if((!chessboard->piece_count[WHITE][KNIGHT_INDEX] )  &&  (!chessboard->piece_count[BLACK][KNIGHT_INDEX] ) ){

              difference=chessboard->piece_count[WHITE][BISHOP_INDEX]-chessboard->piece_count[BLACK][BISHOP_INDEX];
              //abs
              if(difference<2 && difference>=-2){
                   return true;
              }


           }else if(((chessboard->piece_count[WHITE][KNIGHT_INDEX] <3)  &&  (!chessboard->piece_count[WHITE][BISHOP_INDEX] ) )||
                  ((chessboard->piece_count[WHITE][BISHOP_INDEX] ==1 )  &&   (!chessboard->piece_count[WHITE][KNIGHT_INDEX] )  ) ){


                      if(((chessboard->piece_count[BLACK][KNIGHT_INDEX] <3)  &&  (!chessboard->piece_count[BLACK][BISHOP_INDEX] ) )||
                  ((chessboard->piece_count[BLACK][BISHOP_INDEX] ==1 )  &&   (!chessboard->piece_count[BLACK][KNIGHT_INDEX] )  ) ){


                      return true;
                  }


           }


    }else if(  (!chessboard->piece_count[WHITE][QUEEN_INDEX] ) &&
       (!chessboard->piece_count[BLACK][QUEEN_INDEX] )){

           //queen

           if(chessboard->piece_count[WHITE][ROOK_INDEX] ==1 &&  chessboard->piece_count[BLACK][ROOK_INDEX] ==1 ){

               if((chessboard->piece_count[WHITE][KNIGHT_INDEX]+chessboard->piece_count[WHITE][BISHOP_INDEX] <2)&&
                   (chessboard->piece_count[BLACK][KNIGHT_INDEX]+chessboard->piece_count[BLACK][BISHOP_INDEX] <2)){
                    return true;
                   }


           }else if((chessboard->piece_count[WHITE][ROOK_INDEX]==1 )&&(!chessboard->piece_count[BLACK][ROOK_INDEX])){


                     if((chessboard->piece_count[WHITE][KNIGHT_INDEX]+chessboard->piece_count[WHITE][BISHOP_INDEX] ==0)&&
                   ((chessboard->piece_count[BLACK][KNIGHT_INDEX]+chessboard->piece_count[BLACK][BISHOP_INDEX] ==1)||
                       (chessboard->piece_count[BLACK][KNIGHT_INDEX]+chessboard->piece_count[BLACK][BISHOP_INDEX] ==2) )  ){
                    return true;
                   }


           }else if((chessboard->piece_count[BLACK][ROOK_INDEX]==1) &&(!chessboard->piece_count[WHITE][ROOK_INDEX])){

                      if((chessboard->piece_count[BLACK][KNIGHT_INDEX]+chessboard->piece_count[BLACK][BISHOP_INDEX] ==0)&&
                  ((chessboard->piece_count[WHITE][KNIGHT_INDEX]+chessboard->piece_count[WHITE][BISHOP_INDEX] ==1)||
                       (chessboard->piece_count[WHITE][KNIGHT_INDEX]+chessboard->piece_count[WHITE][BISHOP_INDEX] ==2) )  ){
                    return true;
                   }

           }
       }

       return false;
}

void create_attack_tables(const BOARD_STRUCT* chessboard ,const UINT_64 piece_bitboard[2][6],UINT_64 attack_tables[2][6],const UINT_64 all_pieces[],UINT_64 defend_tables[2][6]){


    //bishop queen
     //bishop
    UINT_64 tmp_attack_mask;
    UINT_64 blocker=all_pieces[WHITE]|all_pieces[BLACK];
    UINT_64 intersect;
    UINT_64 all_attack_mask;
    int closest_piece_index;
    int i;
    int square;
    int square_64;
    int len;
    //white and black
    for(int color=0;color<=1;color++){

        len=chessboard->piece_count[color][BISHOP_INDEX];
        for(i=0; i<len; i++)
        {
            all_attack_mask=0ULL;
            square=chessboard->piece_list[color][BISHOP_INDEX][i];
            square_64=BOARD_120_To_64[square];


             //calculate ray and blocker piece and generate attack table
             //north_west
             tmp_attack_mask=direction_masks[NORTH_WEST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=msb64(intersect);
                   tmp_attack_mask&=~direction_masks[NORTH_WEST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

             //north_east
             tmp_attack_mask=direction_masks[NORTH_EAST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=msb64(intersect);
                   tmp_attack_mask&=~direction_masks[NORTH_EAST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //south_west
             tmp_attack_mask=direction_masks[SOUTH_WEST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=lsb64(intersect);
                   tmp_attack_mask&=~direction_masks[SOUTH_WEST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //south_east
             tmp_attack_mask=direction_masks[SOUTH_EAST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=lsb64(intersect);
                   tmp_attack_mask&=~direction_masks[SOUTH_EAST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;
             //set attack mask
             attack_tables[color][BISHOP_INDEX]|=all_attack_mask;
        }
        defend_tables[color][BISHOP_INDEX]= attack_tables[color][BISHOP_INDEX];
        //remove attack mask from friendly pieces
        attack_tables[color][BISHOP_INDEX]&=~all_pieces[color];

    }

    //rook
     //white and black
    for(int color=0;color<=1;color++){

        len=chessboard->piece_count[color][ROOK_INDEX];
        for(i=0; i<len; i++)
        {
            all_attack_mask=0ULL;
            square=chessboard->piece_list[color][ROOK_INDEX][i];
            square_64=BOARD_120_To_64[square];


             //calculate ray and blocker piece and generate attack table
             //north
             tmp_attack_mask=direction_masks[NORTH][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=msb64(intersect);
                   tmp_attack_mask&=~direction_masks[NORTH][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

             //south
             tmp_attack_mask=direction_masks[SOUTH][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=lsb64(intersect);
                   tmp_attack_mask&=~direction_masks[SOUTH][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //west
             tmp_attack_mask=direction_masks[WEST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=msb64(intersect);
                   tmp_attack_mask&=~direction_masks[WEST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //east
             tmp_attack_mask=direction_masks[EAST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=lsb64(intersect);
                   tmp_attack_mask&=~direction_masks[EAST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;
             //set attack mask
             attack_tables[color][ROOK_INDEX]|=all_attack_mask;
        }
        defend_tables[color][ROOK_INDEX]= attack_tables[color][ROOK_INDEX];
        //remove attack mask from friendly pieces
        attack_tables[color][ROOK_INDEX]&=~all_pieces[color];

    }

     for(int color=0;color<=1;color++){

        len=chessboard->piece_count[color][QUEEN_INDEX];
        for(i=0; i<len; i++)
        {
            all_attack_mask=0ULL;
            square=chessboard->piece_list[color][QUEEN_INDEX][i];
            square_64=BOARD_120_To_64[square];


             //calculate ray and blocker piece and generate attack table
             //north_west
             tmp_attack_mask=direction_masks[NORTH_WEST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=msb64(intersect);
                   tmp_attack_mask&=~direction_masks[NORTH_WEST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

             //north_east
             tmp_attack_mask=direction_masks[NORTH_EAST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=msb64(intersect);
                   tmp_attack_mask&=~direction_masks[NORTH_EAST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //south_west
             tmp_attack_mask=direction_masks[SOUTH_WEST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=lsb64(intersect);
                   tmp_attack_mask&=~direction_masks[SOUTH_WEST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //south_east
             tmp_attack_mask=direction_masks[SOUTH_EAST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=lsb64(intersect);
                   tmp_attack_mask&=~direction_masks[SOUTH_EAST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //calculate ray and blocker piece and generate attack table
             //north
             tmp_attack_mask=direction_masks[NORTH][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=msb64(intersect);
                   tmp_attack_mask&=~direction_masks[NORTH][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

             //south
             tmp_attack_mask=direction_masks[SOUTH][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=lsb64(intersect);
                   tmp_attack_mask&=~direction_masks[SOUTH][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //west
             tmp_attack_mask=direction_masks[WEST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=msb64(intersect);
                   tmp_attack_mask&=~direction_masks[WEST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;

              //east
             tmp_attack_mask=direction_masks[EAST][square_64];
             intersect=tmp_attack_mask&blocker;
             if(intersect){
                   closest_piece_index=lsb64(intersect);
                   tmp_attack_mask&=~direction_masks[EAST][closest_piece_index];
             }
             all_attack_mask|=tmp_attack_mask;




             //set attack mask
             attack_tables[color][QUEEN_INDEX]|=all_attack_mask;
        }
        defend_tables[color][QUEEN_INDEX]= attack_tables[color][QUEEN_INDEX];
        //remove attack mask from friendly pieces
        attack_tables[color][QUEEN_INDEX]&=~all_pieces[color];

    }

      //knight
    len=chessboard->piece_count[WHITE][KNIGHT_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[WHITE][KNIGHT_INDEX][i];
        square_64=BOARD_120_To_64[square];
        defend_tables[WHITE][KNIGHT_INDEX]|=knight_attack_table[square_64];
        attack_tables[WHITE][KNIGHT_INDEX]|=knight_attack_table[square_64]&(~all_pieces[WHITE]);

    }

    len=chessboard->piece_count[BLACK][KNIGHT_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[BLACK][KNIGHT_INDEX][i];
        square_64=BOARD_120_To_64[square];
         defend_tables[BLACK][KNIGHT_INDEX]|=knight_attack_table[square_64];
        attack_tables[BLACK][KNIGHT_INDEX]|=knight_attack_table[square_64]&(~all_pieces[BLACK]);
    }

    //pawn
    len=chessboard->piece_count[WHITE][PAWN_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[WHITE][PAWN_INDEX][i];
        square_64=BOARD_120_To_64[square];
        defend_tables[WHITE][PAWN_INDEX]|=white_pawn_attack_mask[square_64];
        attack_tables[WHITE][PAWN_INDEX]|=white_pawn_attack_mask[square_64]&(~all_pieces[WHITE]);
    }

    len=chessboard->piece_count[BLACK][PAWN_INDEX];
    for(i=0; i<len; i++)
    {
        square=chessboard->piece_list[BLACK][PAWN_INDEX][i];
        square_64=BOARD_120_To_64[square];
        defend_tables[BLACK][PAWN_INDEX]|=black_pawn_attack_mask[square_64];
        attack_tables[BLACK][PAWN_INDEX]|=black_pawn_attack_mask[square_64]&(~all_pieces[BLACK]);
    }


    //king attack table
    attack_tables[WHITE][KING_INDEX]=king_inner_ring[BOARD_120_To_64[chessboard->piece_list[WHITE][KING_INDEX][0]]]&(~all_pieces[WHITE]);
    attack_tables[BLACK][KING_INDEX]=king_inner_ring[BOARD_120_To_64[chessboard->piece_list[BLACK][KING_INDEX][0]]]&(~all_pieces[BLACK]);

}

