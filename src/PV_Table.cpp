#include "PV_Table.h"

unsigned int hash_size= 0x100000*256;
void init_hash_table(HASH_TABLE* hash_table){
    hash_table->number_of_entries=hash_size/sizeof(HASH_ENTRY);
    //hash index
    hash_table->number_of_entries-=5;
    hash_table->table.resize(hash_table->number_of_entries);
    clear_table(hash_table);
    printf("hash table size %d \n",hash_table->number_of_entries);
}


void clear_table(HASH_TABLE * pv_table){
    for(unsigned int i=0;i<pv_table->number_of_entries;i++){
        pv_table->table[i].mov=NO_MOVE;
        pv_table->table[i].position_key=0;
        pv_table->table[i].depth=0;
        pv_table->table[i].score=0;
        pv_table->table[i].flags=0;
    }
    pv_table->new_write=0;
}



void store_hash_entry(BOARD_STRUCT * chessboard,const UINT_32 mov,int score,const int flags,const int depth){
    unsigned int hash_table_index=chessboard->position_key%chessboard->hash_table.number_of_entries;
    ASSERT(hash_table_index>=0 && hash_table_index<chessboard->hash_table.number_of_entries);
    ASSERT(depth>0 && depth<MAX_SEARCH_DEPTH);
    ASSERT(flags>=HF_ALPHA && flags<=HF_EXACT);
    ASSERT(score>=-INFINITY && score<= INFINITY );
    ASSERT(chessboard->ply>=0 && chessboard->ply<MAX_SEARCH_DEPTH);

    if(chessboard->hash_table.table[hash_table_index].position_key==0){
        chessboard->hash_table.new_write++;
    }else{
       chessboard->hash_table.over_write++;
    }
    //if mate score reset score to check mate score    28997 to 29000
    if(score>= IS_MATE){
        score=CHECK_MATE_SCORE;
    }else if(score<=-IS_MATE){
        score=-CHECK_MATE_SCORE;
    }

    chessboard->hash_table.table[hash_table_index].mov=mov;
    chessboard->hash_table.table[hash_table_index].position_key=chessboard->position_key;
    chessboard->hash_table.table[hash_table_index].flags=flags,
    chessboard->hash_table.table[hash_table_index].score=score;
    chessboard->hash_table.table[hash_table_index].depth=depth;
}

UINT_32 probe_pv_table(BOARD_STRUCT* chessboard){
    unsigned int hash_table_index=chessboard->position_key%chessboard->hash_table.number_of_entries;

    if(chessboard->hash_table.table[hash_table_index].position_key==chessboard->position_key){
        return chessboard->hash_table.table[hash_table_index].mov;
    }
    return NO_MOVE;
}

bool probe_hash_table(BOARD_STRUCT* chessboard,UINT_32* mov,int*score ,int alpha,int beta,int depth){
     unsigned int hash_table_index=chessboard->position_key%chessboard->hash_table.number_of_entries;
    ASSERT(hash_table_index>=0 && hash_table_index<chessboard->hash_table.number_of_entries);
    ASSERT(depth>0 && depth<MAX_SEARCH_DEPTH);
    ASSERT(alpha<beta);
    ASSERT(alpha>=-INFINITY && alpha<=INFINITY);
    ASSERT(beta>=-INFINITY && beta <=INFINITY);
    ASSERT(chessboard->ply>=0 && chessboard->ply< MAX_SEARCH_DEPTH);


    if(chessboard->hash_table.table[hash_table_index].position_key==chessboard->position_key){
        *mov=chessboard->hash_table.table[hash_table_index].mov;
        if(chessboard->hash_table.table[hash_table_index].depth>=depth){
            chessboard->hash_table.hit++;
            ASSERT(chessboard->hash_table.table[hash_table_index].depth>=1 && chessboard->hash_table.table[hash_table_index].depth<MAX_SEARCH_DEPTH);
            ASSERT(chessboard->hash_table.table[hash_table_index].flags>=HF_ALPHA && chessboard->hash_table.table[hash_table_index].flags<=HF_EXACT);
            *score=chessboard->hash_table.table[hash_table_index].score;


                if(*score>= IS_MATE){
                    *score-=chessboard->ply;
                }else if(*score<=-IS_MATE){
                    *score+=chessboard->ply;
                }

                switch(chessboard->hash_table.table[hash_table_index].flags){
                    case HF_ALPHA:
                        if(*score<=alpha){
                            *score=alpha;
                            return true;
                        }
                        break;
                    case HF_BETA:
                        if(*score>=beta){
                            *score=beta;
                            return true;
                        }
                        break;
                    case HF_EXACT:
                        return true;
                        break;
                    default:
                       ASSERT(false);
                       break;
                }

        }
    }
    return false;
}


int get_pv_line(const int depth,BOARD_STRUCT* chessboard){
    UINT_32 mov=probe_pv_table(chessboard);
    int counter=0;

    while(mov!=NO_MOVE && counter<depth){
        if(is_move_exists(chessboard,mov)){
            make_move_legal(mov,chessboard);
            chessboard->pv_array[counter]=mov;
            counter++;
        }else{
            break;
        }
        mov=probe_pv_table(chessboard);
    }
    for(int i=0;i<counter;i++){
        take_move(chessboard);
    }
    return counter;
}
