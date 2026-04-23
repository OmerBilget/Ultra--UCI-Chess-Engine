#include "PeftTest.h"

unsigned long long leaf_nodes;
void perft_test(int depth,BOARD_STRUCT* chessboard){
    if(depth==0){
        leaf_nodes++;
        return ;
    }
    MOVELIST movelist;
    generate_all_moves(chessboard,&movelist);
    int len=movelist.counter;
    for(int i=0;i<len;i++){
        if(!make_move(movelist.moves[i].id,chessboard)){
            continue;
        }
        perft_test(depth-1,chessboard);
        take_move(chessboard);
    }
}

void divide_perft_test(int depth ,BOARD_STRUCT *chessboard){
    MOVELIST movelist;
    leaf_nodes=0;
    generate_all_moves(chessboard,&movelist);
    int len=movelist.counter;
    unsigned long long divide_node=0;
    unsigned long long tmpnode=0;
    for(int i=0;i<len;i++){
        if(!make_move(movelist.moves[i].id,chessboard)){
            continue;
        }
        tmpnode=leaf_nodes;
        perft_test(depth-1,chessboard);
        divide_node=leaf_nodes-tmpnode;
        display(FROM_ENCODE(movelist.moves[i].id));
        display(TO_ENCODE(movelist.moves[i].id));
        printf(" %d -%llu\n",i,divide_node);
        take_move(chessboard);
    }
    printf("\n leaf nodes %llu\n",leaf_nodes);
}

void perft_test_legal_moves(int depth,BOARD_STRUCT* chessboard){
    if(depth==0){
        leaf_nodes++;
        return ;
    }
    MOVELIST movelist;
    generate_legal_moves(chessboard,&movelist);
    int len=movelist.counter;
    for(int i=0;i<len;i++){
        make_move_legal(movelist.moves[i].id,chessboard);
        perft_test(depth-1,chessboard);
        take_move(chessboard);
    }
}

void divide_perft_test_legal_moves(int depth ,BOARD_STRUCT *chessboard){
    MOVELIST movelist;
    leaf_nodes=0;
    generate_legal_moves(chessboard,&movelist);
    int len=movelist.counter;
    unsigned long long divide_node=0;
    unsigned long long tmpnode=0;
    for(int i=0;i<len;i++){
        make_move_legal(movelist.moves[i].id,chessboard);
        tmpnode=leaf_nodes;
        perft_test(depth-1,chessboard);
        divide_node=leaf_nodes-tmpnode;
        display(FROM_ENCODE(movelist.moves[i].id));
        display(TO_ENCODE(movelist.moves[i].id));
        printf(" %d -%llu\n",i,divide_node);
        take_move(chessboard);
    }
    printf("\n leaf nodes %llu\n",leaf_nodes);
}


void display(int num){
    char coordinate[3];
    coordinate[2]='\0';
    int h=(num-1)%10;
    int v=7-((num/10)-2);
    coordinate[0]='a'+h;
    coordinate[1]='1'+v;
    printf("%s",coordinate);
}


