#include "Fen.h"

bool is_digit (char c){
    return  ( c=='0'|| c=='1'|| c=='2' || c=='3'||  c=='4'||   c=='5'||  c=='6'||  c=='7'||  c=='8'||  c=='9' );
}


bool parse_fen_string(std::string fen,BOARD_STRUCT * chessboard){
    std::vector<std::string> split_str;
    split_str.reserve(10);

    init_board(chessboard);
    reset_board(chessboard);
    //split fen str by space
    split_string_by_delimiter(fen," ",split_str);


    //check if fen str is true
    int split_str_len=split_str.size();
    if(split_str_len!=6 && split_str_len!=4){
        //printf("fen str size error\n");
        return false;
    }


    //split first by "/"
    std::vector<std::string> split_str_pos;
    split_str_pos.reserve(10);
    split_string_by_delimiter(split_str[0],"/",split_str_pos);

     int  split_str_pos_size=split_str_pos.size();


    int board_row_counter;
    int row_length;
    //if not match return false
    if(split_str_pos_size!=8){
        //printf("error / split size dont match\n");
        return false;
    }
    int num;
    for(int i=0;i<split_str_pos_size;i++){
        board_row_counter=0;
        row_length=split_str_pos[i].length();
        for(int j=0;j<row_length;j++){

            char c=split_str_pos[i][j];
            if(board_row_counter>7){
                // printf("error / split size row count bigger than max limit at row %d   count %d\n",i,board_row_counter);
                 return false;
            }
            switch(c){
                case 'P' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=WHITE_PAWN;
                    board_row_counter++;
                    break;
                case 'R' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=WHITE_ROOK;
                    board_row_counter++;
                    break;
                case 'N' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=WHITE_KNIGHT;
                    board_row_counter++;
                    break;
                case 'B' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=WHITE_BISHOP;
                    board_row_counter++;
                    break;
                case 'Q' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=WHITE_QUEEN;
                    board_row_counter++;
                    break;
                case 'K' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=WHITE_KING;
                    board_row_counter++;
                    break;
                 case 'p' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=BLACK_PAWN;
                    board_row_counter++;
                    break;
                case 'r' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=BLACK_ROOK;
                    board_row_counter++;
                    break;
                case 'n' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=BLACK_KNIGHT;
                    board_row_counter++;
                    break;
                case 'b' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=BLACK_BISHOP;
                    board_row_counter++;
                    break;
                case 'q' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=BLACK_QUEEN;
                    board_row_counter++;
                    break;
                case 'k' :
                    chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=BLACK_KING;
                    board_row_counter++;
                    break;
                default:
                    if(is_digit(c)){
                        num=char_to_int(c);
                        for(int k=0; k<num; k++)
                        {
                             if(board_row_counter>7){
                                   // printf("error / split size row count bigger than max limit at row %d   count %d\n",i,board_row_counter);
                                    return false;
                            }
                            chessboard->board[BOARD_64_To_120[i*8+board_row_counter]]=EMPTY;
                            board_row_counter++;
                        }
                    }else{
                        //non recognized char return error
                        //printf("illegal position char\n");
                        return false;
                    }
            }
        }

        if(board_row_counter!=8){
            //printf("error board row counter not 8  at row %d\n",i);
            return false;
        }

    }


    //black  and white
    if(split_str[1]=="w"){
        chessboard->side=WHITE;
    }else if(split_str[1]=="b"){
        chessboard->side=BLACK;
    }else{
        //printf("error b w\n");
        return false;
    }





     chessboard->castle_permissions=0;
    //castling
    //0 K   1 Q   2 k   3 q
    bool castling_match[4]={false};
    if(split_str[2]=="-"){
        chessboard->castle_permissions=0;
    }else{
        split_str_len=split_str[2].length();
        if(split_str_len>4){
            //printf("castling str size don't match\n");
            return false;
        }
        for(int i=0;i<split_str_len;i++){
            char c=split_str[2][i];
            switch(c){
                case 'K':
                    if(castling_match[0]){
                      //  printf("repeat castling character K\n");
                        return false;
                    }else{
                        castling_match[0]=true;
                          for(int i=3;i>0;i--){
                              if(castling_match[i]){
                                //   printf("castling order error K\n");
                                   return false;
                              }
                          }

                        chessboard->castle_permissions|=WHITE_KING_CASTLE;
                    }
                    break;
                case 'Q':
                    if(castling_match[1]){
                    //    printf("repeat castling character Q\n");
                        return false;
                    }else{
                        castling_match[1]=true;
                        for(int i=3;i>1;i--){
                              if(castling_match[i]){
                               //    printf("castling order error Q\n");
                                   return false;
                              }
                          }
                        chessboard->castle_permissions|=WHITE_QUEEN_CASTLE;
                    }
                    break;
                case 'k':
                    if(castling_match[2]){
                      //  printf("repeat castling character k\n");
                        return false;
                    }else{
                        if(castling_match[3]){
                        //    printf("castling order error k\n");
                            return false;
                        }
                        castling_match[2]=true;
                        chessboard->castle_permissions|=BLACK_KING_CASTLE;
                    }
                    break;
                case 'q':
                    if(castling_match[3]){
                   //     printf("repeat castling character q\n");
                        return false;
                    }else{
                        castling_match[3]=true;
                        chessboard->castle_permissions|=BLACK_QUEEN_CASTLE;
                    }
                    break;
                default:
                 //   printf("%d castle unknown char\n");
                    return false;
            }
        }
    }


    //en passant
    int square;
    if(split_str[3]=="-"){
        chessboard->enPassant_square=NO_EP;
    }else{
        square=convert_file_ranks_to_square(split_str[3]);
        if(square==-1){
         //   printf("en_passant_error\n");
            return false;
        }
        if(chessboard->side==WHITE){
            if(square>=41 && square <=48){
                chessboard->enPassant_square=square;
            }else{
                //printf("error ep\n");
                return false;
            }
        }else if(chessboard->side==BLACK){
             if(square>=71 && square <=78){
                chessboard->enPassant_square=square;
            }else{
                //printf("error ep\n");
                return false;
            }
        }
    }

    //no ply or fifty rule
    split_str_len=split_str.size();
    int counter;
    int len;
    int num_convert;
    if(split_str_len==4){
        chessboard->fiftyMoveCounter=0;
        chessboard->ply=0;
    }else{

        //fifty move counter
        counter=1;
        len=split_str[4].length();
        num=0;
        for(int i=len-1;i>=0;i--){
            num_convert=char_to_int(split_str[4][i]);
            if(num_convert==-1){
             //   printf("char is not a number at fifty move rule\n");
                return false;
            }
            num+=counter*num_convert;
            counter*=10;
        }
        if(num<0){
            return false;
        }
        chessboard->fiftyMoveCounter=num;

        //
        counter=1;
        len=split_str[5].length();
        num=0;
        for(int i=len-1;i>=0;i--){
            num_convert=char_to_int(split_str[5][i]);
            if(num_convert==-1){
            //    printf("char is not a number at ply\n");
                return false;
            }
            num+=counter*num_convert;
            counter*=10;
        }
        if(num<0){
            return false;
        }
        chessboard->ply=num;

    }

    fill_lists(chessboard);
    fill_materials(chessboard);
    chessboard->position_key=generate_position_key(chessboard);
    return true;
}
