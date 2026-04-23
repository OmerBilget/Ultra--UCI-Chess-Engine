#include "Search.h"
#include "IO.h"



//if null move pruning enabled
#define NULL_MOVE_PRUNING
//if reduction enabled instead of cutoff reduce depth
#define NULL_MOVE_REDUCTION

//if late move reduction enabled
#define LATE_MOVE_REDUCTION

#define LATE_MOVE_REDUCTION_DEPTH 3

//null move pruning
#define MAX_R 4
#define MIN_R 3
#define DEPTH_REDUCTION 4

#define MIN_SEARCH_LIMIT 3
#define TIMER_INTERVAL 8191

//enable principle variation search
#define PRINCIPAL_VARIATION_SEARCH
inline bool is_repetition(const BOARD_STRUCT * chessboard){
    int i;
    //if pawn is moved or capture is made fifty move counter reset
    //these moves cant be repeated so it is sufficient to look back only fifty move counter amount in history array
    int start_index=chessboard->history_counter-chessboard->fiftyMoveCounter;
    int end_index=chessboard->history_counter-1;
    for(i=start_index;i<end_index;i++){
        if(chessboard->history[i].position_key==chessboard->position_key){
            return true;
        }
    }
    return false;
}

//iterative deepening
void search_position(BOARD_STRUCT* chessboard,SEARCH_INFORMATION * search_information){

    UINT_32 best_move=NO_MOVE;
    int best_score=-INFINITY;
    int current_depth=0;
    int pv_moves=0;

    clear_for_search(chessboard,search_information);

    for(current_depth=1;current_depth<=search_information->depth_limit;current_depth++){
        best_score=alpha_beta(-INFINITY,INFINITY,current_depth,chessboard,search_information,true);
        //out of time
        if(search_information->stopped){
            break;
        }

        pv_moves=get_pv_line(current_depth,chessboard);
        best_move=chessboard->pv_array[0];
        printf("info score cp score %d depth %d nodes %ld time %llu ",best_score,current_depth,search_information->nodes,millis()-search_information->start_time);
        //std::cout<<convert_move_to_string(best_move)<<"   ";


        //print pv
        printf("pv ");
        for(int i=0;i<pv_moves;i++){
            std::cout<<convert_move_to_string(chessboard->pv_array[i])<<" ";
        }
        printf("\n order %.2f",search_information->fail_high_first/search_information->fail_high);
        printf(" cut %d  hit %d  new:%d  overwrite:%d", chessboard->hash_table.cut,chessboard->hash_table.hit,chessboard->hash_table.new_write,chessboard->hash_table.over_write);
        printf("\n");

    }

    std::cout<<"bestmove "<<convert_move_to_string(best_move)<<"\n";
}

void clear_for_search(BOARD_STRUCT* chessboard,SEARCH_INFORMATION * search_information){
    int i,j,k;

    for(i=0;i<2;i++){
        for(j=0;j<6;j++){
            for(k=0;k<BOARD_LENGTH;k++){
                chessboard->search_history[i][j][k]=0;
            }
        }
    }

    for(i=0;i<2;i++){
        for(j=0;j<MAX_SEARCH_DEPTH;j++){
            chessboard->search_killer_moves[i][j]=0;
        }
    }


    chessboard->ply=0;

    //reset hash table values
    chessboard->hash_table.over_write=0;
     chessboard->hash_table.cut=0;
     chessboard->hash_table.hit=0;


    search_information->stopped=false;
    search_information->nodes=0;
    search_information->fail_high=0.0f;
    search_information->fail_high_first=0.0f;
}

int alpha_beta(int alpha,int beta,int depth,BOARD_STRUCT* chessboard,SEARCH_INFORMATION * search_information,bool do_null_move){

    if(depth<=0){
        search_information->nodes++;
        return quiescense_search(alpha,beta,chessboard,search_information);
    }


    if((search_information->nodes & TIMER_INTERVAL)==0){
        check_timer(search_information);
    }

    search_information->nodes++;

    //if repetition or fifty move rule game is draw return 0;
    if((is_repetition(chessboard) || chessboard->fiftyMoveCounter>=100)&& chessboard->ply!=0){
            return 0;
    }

    //depth limit reached
    if(chessboard->ply>MAX_SEARCH_DEPTH-1){
        return evaluation(chessboard);
    }


     int R_VAL;
     #ifdef LATE_MOVE_REDUCTION
     int late_reduction_val;
     #endif // LATE_MOVE_REDUCTION
     bool is_incheck;
     bool is_enemy_incheck;
     int score= -INFINITY;

     UINT_32 pv_move=NO_MOVE;
     //if end of the search if alpha is changed store history move
    int old_alpha=alpha;
    UINT_32 best_move=NO_MOVE;


    //if is in check that might lead to checkmate so increase depth by one and search next depth to see real value of move
    is_incheck=is_square_attacked(chessboard->piece_list[chessboard->side][KING_INDEX][0],chessboard->side^1,chessboard);
    if(is_incheck){
         depth++;
    }

    if(probe_hash_table(chessboard,&pv_move,&score,alpha,beta,depth)){
        chessboard->hash_table.cut++;
        return score;
    }




   //null move pruning
  #ifdef NULL_MOVE_PRUNING
    if(do_null_move && chessboard->ply &&
       (chessboard->piece_count[chessboard->side][ROOK] ||
        chessboard->piece_count[chessboard->side][BISHOP] ||
        chessboard->piece_count[chessboard->side][KNIGHT] ||
        chessboard->piece_count[chessboard->side][QUEEN]  ) && !is_incheck && depth>= 4){

           R_VAL=depth>6?MAX_R:MIN_R;
           make_null_move(chessboard);
           score=-alpha_beta(-beta,-beta+1,depth - R_VAL-1,chessboard,search_information,false);
           take_null_move(chessboard);
           if(search_information->stopped){
                return 0;
           }
           if(score>=beta){
                #ifdef NULL_MOVE_REDUCTION
                depth-=DEPTH_REDUCTION;
                if(depth<=0){
                    return quiescense_search(alpha,beta,chessboard,search_information);
                }
                #else
                return beta;
                #endif // NULL_MOVE_REDUCTION
           }

    }

    score= -INFINITY;
  #endif // NULL_MOVE_PRUNING



    MOVELIST movelist;
    generate_legal_moves(chessboard,&movelist);



    #ifdef PRINCIPAL_VARIATION_SEARCH
    bool pv_found=false;
    #endif // PRINCIPAL_VARIATION_SEARCH


     //move from previous search iterative deepening
    if(pv_move!=NO_MOVE){
        for(int i=0;i<movelist.counter;i++){
            if(pv_move==movelist.moves[i].id){
                //search first

                movelist.moves[i].score=2000000;
                break;
            }
        }
    }


    int move_count=movelist.counter;
    int piece;
    int best_score=-INFINITY;

    for(int i=0;i<move_count;i++){
        pick_next_move(i,&movelist);
        make_move_legal(movelist.moves[i].id,chessboard);
        #ifdef LATE_MOVE_REDUCTION
        //conditions depth >=3
        //no pawn move
        //no in check and no check move
        //ordered by 3

        is_enemy_incheck=is_square_attacked(chessboard->piece_list[chessboard->side][KING_INDEX][0],chessboard->side^1,chessboard);
        if( i>=MIN_SEARCH_LIMIT){
            if(!(movelist.moves[i].id&FLAG_IS_CAPTURED) &&
           !(movelist.moves[i].id&FLAG_IS_PROMOTED) &&
           !(movelist.moves[i].id&FLAG_CASTLE) &&
           !is_incheck && !is_enemy_incheck &&
           depth>=LATE_MOVE_REDUCTION_DEPTH  &&
           !(FROM_ENCODE(movelist.moves[i].id)==FROM_ENCODE(chessboard->search_killer_moves[0][chessboard->ply]) &&
            TO_ENCODE(movelist.moves[i].id)==FROM_ENCODE(chessboard->search_killer_moves[0][chessboard->ply]) ) &&
            !(FROM_ENCODE(movelist.moves[i].id)==FROM_ENCODE(chessboard->search_killer_moves[1][chessboard->ply]) &&
            TO_ENCODE(movelist.moves[i].id)==FROM_ENCODE(chessboard->search_killer_moves[1][chessboard->ply]) ) &&
           (chessboard->board[FROM_ENCODE(movelist.moves[i].id)]&PIECE_MASK)!=PAWN){

                 late_reduction_val=i>6?2:1;

                score=-alpha_beta(-beta,-alpha,depth-1-late_reduction_val,chessboard,search_information,true);
                //research

           }else{
               score=alpha+1;
           }
           if(score>alpha){
               score=-alpha_beta(-(alpha+1),-alpha,depth-1,chessboard,search_information,true);
               if(score>alpha && score<beta){
                   score=-alpha_beta(-beta,-alpha,depth-1,chessboard,search_information,true);
               }
           }
        }else{
            #ifdef PRINCIPAL_VARIATION_SEARCH
               if(pv_found){
                   score=-alpha_beta(-alpha-1,-alpha,depth-1,chessboard,search_information,true);
                   if(score>alpha && score<beta){
                        score=-alpha_beta(-beta,-alpha,depth-1,chessboard,search_information,true);
                   }
               }else{
                   score=-alpha_beta(-beta,-alpha,depth-1,chessboard,search_information,true);
               }

            #else
                    score=-alpha_beta(-beta,-alpha,depth-1,chessboard,search_information,true);
            #endif // PRINCIPAL_VARIATION_SEARCH

        }

        #else
        score=-alpha_beta(-beta,-alpha,depth-1,chessboard,search_information,true);
        #endif


        take_move(chessboard);

        if(search_information->stopped){
            return 0;
        }
        if(score>best_score){
            best_score=score;
            best_move=movelist.moves[i].id;

            if(score>alpha){

                //beta cutoff
                if(score>=beta){
                    if(i==0){
                        search_information->fail_high_first++;
                    }
                    search_information->fail_high++;

                    //if non capture
                    if(!(movelist.moves[i].id&FLAG_IS_CAPTURED)){
                        chessboard->search_killer_moves[1][chessboard->ply]=chessboard->search_killer_moves[0][chessboard->ply];
                        chessboard->search_killer_moves[0][chessboard->ply]=movelist.moves[i].id;
                    }
                    //store hash entry
                    store_hash_entry(chessboard,best_move,beta,HF_BETA,depth);
                    return beta;
                }

                alpha=score; //change alpha
                best_move=movelist.moves[i].id; //update best move
                #ifdef PRINCIPAL_VARIATION_SEARCH
                pv_found=true;
                #endif // PRINCIPAL_VARIATION_SEARCH
                //alpha change
                //update history moves
                if(!(movelist.moves[i].id&FLAG_IS_CAPTURED)){
                    piece=chessboard->board[FROM_ENCODE(movelist.moves[i].id)];
                    chessboard->search_history[piece>>PIECE_ROTATE][piece&PIECE_MASK][TO_ENCODE(movelist.moves[i].id)]+=depth;
                }
            }
        }


    }

    if(move_count==0){
        if(is_incheck){
            //lose the game return negative score
            //ply or current search depth is used because sooner the checkmate  is better
            return -CHECK_MATE_SCORE + chessboard->ply;
        }else{
            return 0;
        }
    }

    if(old_alpha!=alpha){
        //if alpha is changed store best move that changes alpha but not cause beta cutoff
         store_hash_entry(chessboard,best_move,best_score,HF_EXACT,depth);
    }else{
         store_hash_entry(chessboard,best_move,alpha,HF_ALPHA,depth);
    }


    return alpha;
}

int quiescense_search(int alpha,int beta,BOARD_STRUCT* chessboard,SEARCH_INFORMATION * search_information){

    if((search_information->nodes & TIMER_INTERVAL)==0){
        check_timer(search_information);
    }
    search_information->nodes++;

    //if repetition or fifty move rule game is draw return 0;
    if((is_repetition(chessboard) || chessboard->fiftyMoveCounter>=100)&& chessboard->ply!=0){
            return 0;
    }


    //depth limit reached
    if(chessboard->ply>MAX_SEARCH_DEPTH-1){
        return evaluation(chessboard);
    }

    int score=evaluation(chessboard);
    if(score>=beta){
        return beta;
    }
    if(score>alpha){
        alpha=score;
    }

    MOVELIST movelist;
    generate_legal_capture_moves(chessboard,&movelist);


    int capture_count=movelist.counter;
    for(int i=0;i<capture_count;i++){
         pick_next_move(i,&movelist);
        make_move_legal(movelist.moves[i].id,chessboard);
        score=-quiescense_search(-beta,-alpha,chessboard,search_information);
        take_move(chessboard);
        if(search_information->stopped){
            return 0;
        }
        if(score>alpha){

            //beta cutoff
            if(score>=beta){
                if(i==0){
                    search_information->fail_high_first++;
                }
                search_information->fail_high++;
                return beta;
            }

            alpha=score; //change alpha
        }

    }

    return alpha;
}

//check if search time is up or interrupt from ui
inline void check_timer(SEARCH_INFORMATION* search_information){
    //if search is time limited and time is out stop search
    if(search_information->time_set && millis()>search_information->end_time){
        search_information->stopped=true;
    }
    //check if input buffer is up
    ReadInput(search_information);
}

//move ordering
inline void pick_next_move(int move_num,MOVELIST* movelist){
    MOVE tmp;
    int i;
    int best_score=0;
    int best_move_num=move_num;
    for(i=move_num;i<movelist->counter;i++){
        if(movelist->moves[i].score>best_score){
            best_score=movelist->moves[i].score;
            best_move_num=i;
        }
    }

    //swap current move with best move
    tmp=movelist->moves[move_num];
    movelist->moves[move_num]=movelist->moves[best_move_num];
    movelist->moves[best_move_num]=tmp;
}

