#include "UCI_Protocol.h"


void uci_loop(BOARD_STRUCT* chessboard,SEARCH_INFORMATION* search_information)
{
    std::string line;
    std::vector<std::string> line_split{};
    bool success=false;
    while(!search_information->quit)
    {
        std::getline(std::cin,line);
        split_string_by_delimiter(line," ",line_split);

        //commands
        if(line_split[0]=="go")
        {
            success=parse_go(line,chessboard,search_information);
             if(success){

               search_position(chessboard,search_information);
            }
        }
        else if(line_split[0]=="position")
        {
            success=parse_position(line,chessboard);
            if(success){
               // print_chessboard(chessboard->board);
            }
        }
        else if(line_split[0]=="ucinewgame")
        {
            success=parse_position("position startpos",chessboard);

            if(success){
              //  print_chessboard(chessboard->board);
            }
        }
        else if(line_split[0]=="isready")
        {
            std::cout<<"readyok"<<"\n";
        }
        else if(line_split[0]=="quit")
        {
            search_information->quit=true;
            break;
        }
        else if(line_split[0]=="uci")
        {
            std::cout<<"id name "<<NAME<<"\n";
            std::cout<<"id author "<<AUTHOR<<"\n";
            std::cout<<"uciok"<<"\n";
        }else if(line_split[0]=="disp")
        {
            print_chessboard(chessboard->board);
        }
    }
}



bool parse_go(std::string line,BOARD_STRUCT* chessboard,SEARCH_INFORMATION* search_information)
{
    std::vector<std::string> words{};
    split_string_by_delimiter(line," ",words);
    int len =words.size();
    int depth=-1;
    int moves_to_go=30;
    int movetime=-1;
    int time =-1;
    int inc =0;
    search_information->time_set=false;
    if(len==1){
        return false;
    }


    for(int i=1;i<len;i++){
        if(words[i]=="infinite"){
            search_information->time_set=false;
        }else if(words[i]=="winc" && chessboard->side==WHITE){
            if(i==len-1){
                return false;
            }
             inc=convert_str_to_int(words[i+1]);
             if(inc==-1){
                printf("winc error\n");
                return false;
             }
        }else if(words[i]=="binc"  && chessboard->side==BLACK){
             if(i==len-1){
                return false;
            }
             inc=convert_str_to_int(words[i+1]);
             if(inc==-1){
                printf("winc error\n");
                return false;
             }
        }else if(words[i]=="wtime"  && chessboard->side==WHITE){
             if(i==len-1){
                return false;
            }
              time=convert_str_to_int(words[i+1]);
             if(time==-1){
                printf("wtime error\n");
                return false;
             }
        }else if(words[i]=="btime"   && chessboard->side==BLACK){
             if(i==len-1){
                return false;
            }
             time=convert_str_to_int(words[i+1]);
             if(time==-1){
                printf("btime error\n");
                return false;
             }
        }else if(words[i]=="movestogo"){
            if(i==len-1){
                return false;
            }
             moves_to_go=convert_str_to_int(words[i+1]);
             if(moves_to_go==-1){
                printf("movestogo error\n");
                return false;
             }
        }else if(words[i]=="movetime"){



            if(i==len-1){
                return false;
            }
             movetime=convert_str_to_int(words[i+1]);
             if(movetime==-1){
                printf("move_time error\n");
                return false;
             }



        }else if(words[i]=="depth"){
            if(i==len-1){
                return false;
            }
            depth=convert_str_to_int(words[i+1]);
             if(depth==-1){
                printf("depth error\n");
                return false;
             }
        }
    }

    if(movetime!=-1){
        time=movetime;
        moves_to_go=1;
    }

    search_information->start_time=millis();
    search_information->depth_limit=depth;


    if(time!=-1){
        search_information->time_set=true;
        time/=moves_to_go;
        //not overrun
        time-=50;
        search_information->end_time=search_information->start_time+time+inc;
    }
    if(depth==-1){
        search_information->depth_limit=MAX_SEARCH_DEPTH;
    }
    printf("time %d start %llu stop %llu depth %d timeset %d \n",time,search_information->start_time,search_information->end_time,search_information->depth_limit,search_information->time_set);
    return true;
}

bool parse_position(std::string line,BOARD_STRUCT* chessboard)
{
    std::vector<std::string> words{};
    split_string_by_delimiter(line," ",words);
    std::string fen;
    int len;
    int moves_index=-1;
    bool is_parsed=false;
    MOVELIST movelist;
    UINT_32 mov;
    len=words.size();

    if(words[1]=="fen")
    {



        if(len==2)
        {
            return false;
        }


        for(int i=2; i<len; i++)
        {
            if(words[i]=="moves")
            {
                moves_index=i;
                break;
            }
            fen+=words[i];
            fen+=" ";
        }


        is_parsed=parse_fen_string(fen,chessboard);
        if(!is_parsed)
        {
            return false;
        }


        if(moves_index!=-1)
        {
            if(moves_index==len-1)
            {
                return false;
            }
            for(int i=moves_index+1; i<len; i++)
            {
                generate_legal_moves(chessboard,&movelist);
                mov=find_move_string(words[i],&movelist);
                if(mov!=NO_MOVE)
                {
                    make_move_legal(mov,chessboard);
                }
                else
                {
                    return false;
                }
            }
            return true;
        }
        else
        {
            return true;
        }


    }
    else if(words[1]=="startpos")
    {
        is_parsed =parse_fen_string(START_FEN,chessboard);
        for(int i=2; i<len; i++)
        {
            if(words[i]=="moves")
            {
                moves_index=i;
                break;
            }
        }
        if(moves_index!=-1)
        {
            if(moves_index==len-1)
            {
                return false;
            }
            for(int i=moves_index+1; i<len; i++)
            {
                generate_legal_moves(chessboard,&movelist);
                mov=find_move_string(words[i],&movelist);
                if(mov!=NO_MOVE)
                {
                    make_move_legal(mov,chessboard);
                }
                else
                {
                    printf("error");
                    return false;
                }
            }
            return is_parsed;
        }
        else
        {
            return is_parsed;
        }
    }

    return false;
}

