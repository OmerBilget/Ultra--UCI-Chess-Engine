#include "IO.h"

void print_move(UINT_32 mov){
    if(mov==NO_MOVE){
        printf("error\n");
        return;
    }
    int from=FROM_ENCODE(mov);
    int to=TO_ENCODE(mov);
    int capture= CAPTURE(mov);
    int promote=PROMOTE(mov);
    bool ep_flag=((mov&FLAG_EP)!=0);
    bool pawn_start=((mov&FLAG_PAWN_START)!=0);
    bool castle=((mov&FLAG_CASTLE)!=0);
    print_square_to_file_ranks(from);
    print_square_to_file_ranks(to);
    printf(" from:%d  to:%d   capture:%d  promote:%d  ep: %d   castle: %d   pawn start:%d\n",from,to,capture,promote,ep_flag,castle,pawn_start);
}

void print_square_to_file_ranks(int square){
    char coordinate[3];
    coordinate[2]='\0';
    int h=(square-1)%10;
    int v=7-((square/10)-2);
    coordinate[0]='a'+h;
    coordinate[1]='1'+v;
    printf("%s",coordinate);
}

void print_movelist(MOVELIST* movelist){
    int length=movelist->counter;
    printf("Movelist\n");
    for(int i=0;i<length;i++){
        printf("%d ",i);
        print_move(movelist->moves[i].id);
    }
}

//convert move to string
std::string convert_move_to_string(UINT_32 mov){

    std:: string move_str = "";
    int from=FROM_ENCODE(mov);
    int to=TO_ENCODE(mov);
    int promoted_piece=PROMOTE(mov);
    int promoted_piece_type;

    if(!is_square_on_board(from) || !is_square_on_board(to) || (promoted_piece!=0 && !is_piece_valid(promoted_piece)) ){
        move_str= "error";
        return move_str;
    }
    //move_str.find("st")!=std::string::npos
    move_str=convert_square_to_file_ranks(from)+convert_square_to_file_ranks(to);
    if(promoted_piece!=EMPTY){
        promoted_piece_type=promoted_piece&PIECE_MASK;
        if(promoted_piece_type==KNIGHT){
            move_str+="n";
        }else if(promoted_piece_type==ROOK){
            move_str+="r";
        }else if(promoted_piece_type==BISHOP){
            move_str+="b";
        }else if(promoted_piece_type==QUEEN){
            move_str+="q";
        }

    }
    return move_str;
}


//if given move string is valid and move is inside movelist returns move
UINT_32 find_move_string(std::string mov_str,const MOVELIST * movelist){

    //remove spaces
    mov_str.erase(std::remove_if(mov_str.begin(), mov_str.end(), isspace), mov_str.end());


     if((int)mov_str.length()!=5  &&  (int)mov_str.length()!=4){
         #ifdef DEBUG
        printf(" %d length error",(int)mov_str.length());
         #endif // DEBUG
        return NO_MOVE;
    }
    //check if move string valid
    if(mov_str[0]<'a' || mov_str[0]>'h' || mov_str[1]<'1' || mov_str[1]>'8' || mov_str[2]<'a' || mov_str[2]>'h' || mov_str[3]<'1' || mov_str[3]>'8' ){
        #ifdef DEBUG
        printf("file ranks error");
         #endif // DEBUG
        return NO_MOVE;
    }

    if((int)mov_str.length()==5){
       if(mov_str[4]!='n'  && mov_str[4]!='r' && mov_str[4]!='b' && mov_str[4]!='q' ){
        #ifdef DEBUG
        printf("promote piece error");
        #endif // DEBUG
           return NO_MOVE;
       }
    }
    std::string from_str="  ";
    std::string to_str="  ";

    from_str[0]=mov_str[0];
    from_str[1]=mov_str[1];
    to_str[0]=mov_str[2];
    to_str[1]=mov_str[3];

    int from=convert_file_ranks_to_square(from_str);
    int to=convert_file_ranks_to_square(to_str);

    //promote
    int promote=0;
    if((int)mov_str.length()==5){
        switch(mov_str[4]){
            case 'n':promote=KNIGHT;break;
            case 'r':promote=ROOK;break;
            case 'b':promote=BISHOP;break;
            case 'q':promote=QUEEN;break;
        }
    }

   // printf("from to :%d %d\n",from,to);


    int move_from;
    int move_to;
    int move_promote;
    UINT_32 mov;
    for(int i=0;i<movelist->counter;i++){
       mov=movelist->moves[i].id;
       move_from=FROM_ENCODE(mov);
       move_to=TO_ENCODE(mov);
       move_promote=PROMOTE(mov);

       if(from==move_from && to==move_to){
           if(move_promote!=0){
                if((move_promote&PIECE_MASK)==promote){
                    return mov;
                }
           }else{
               return mov;
           }
       }
    }

    return NO_MOVE;
}


std::string convert_square_to_file_ranks(int square){
    std::string coordinate="  ";
     int h=(square-1)%10;
     int v=7-((square/10)-2);
     coordinate[0]='a'+h;
     coordinate[1]='1'+v;
     return coordinate;
}

int convert_file_ranks_to_square(std::string file_ranks_str){
    char file=file_ranks_str[0];
    char ranks=file_ranks_str[1];
    int h=0;
    int v=0;
    int square=-1;
    switch(file){
        case 'a': h=0; break;
        case 'b': h=1; break;
        case 'c': h=2; break;
        case 'd': h=3; break;
        case 'e': h=4; break;
         case 'f': h=5; break;
        case 'g': h=6; break;
        case 'h': h=7; break;
        default:return square;
    }
    switch(ranks){
        case '1': v=7; break;
        case '2': v=6; break;
        case '3': v=5; break;
        case '4': v=4; break;
        case '5': v=3; break;
        case '6': v=2; break;
        case '7': v=1; break;
        case '8': v=0; break;
        default:return square;
    }
    square=BOARD_64_To_120[v*8+h];
    return square;
}


void split_string_by_delimiter(std::string line,std::string delimiter,std::vector<std::string>& split_array){
    split_array={};
    line+=delimiter;
    size_t pos = 0;
    while ((pos = line.find(delimiter)) != std::string::npos) {
        split_array.push_back(line.substr(0, pos));
        line.erase(0, pos +delimiter.length());
    }

    for(int i=0;i<(int)split_array.size();i++){
        if(split_array[i]==""){
            split_array.erase(split_array.begin() + i);
            i--;
        }
    }
}
int char_to_int(char c){
    char ch='0';
    for(int i=0;i<=9;i++){
        if(ch+i==c){
            return i;
        }
    }
    return -1;
}

int convert_str_to_int(std::string str){
    int num=0;
    int counter=1;
    int digit;
    int len=str.length();
    if(len==0){
        return -1;
    }
    for(int i=len-1;i>=0;i--){
        digit=char_to_int(str[i]);
        if(digit==-1){
            return -1;
        }
        num+=counter*digit;
        counter*=10;
    }
    return num;
}

void print_chessboard(int board[])
{
    printf("\n");
    for(int i=2; i<HEIGHT-2; i++)
    {
        printf("%d ",10-i);
        for(int j=1; j<WIDTH-1; j++)
        {
            switch(board[i*WIDTH+j])
            {
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
        printf("\n");
    }
    printf("\n");
    printf("  A B C D E F G H\n");
    printf("\n");
}

void print_board_numbers(int board[]){
    for(int i=0;i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            printf("%d\t",board[i*WIDTH+j]);
        }
        printf("\n\n\n");
    }


    printf("\n\n\n\n\n\n");

    for(int i=2;i<HEIGHT-2;i++){
        for(int j=1;j<WIDTH-1;j++){
            printf("%d\t",board[i*WIDTH+j]);
        }
        printf("\n\n\n");
    }
}

void print_lists(BOARD_STRUCT*  chessboard){
    int i,j,k;

    for(i=0;i<2;i++){
        for(j=0;j<6;j++){
                printf("\n-------%d %d-------\n\n",i,j);
            for(k=0;k<chessboard->piece_count[i][j];k++){
                printf("%d-      %d\n",k,chessboard->piece_list[i][j][k]);
            }
        }
    }
}




