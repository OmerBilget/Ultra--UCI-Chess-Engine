#ifndef PV_TABLE_H
#define PV_TABLE_H

#include "definitions.h"
#include "Movegen.h"
extern unsigned int pv_size;
void init_hash_table(HASH_TABLE*);
void clear_table(HASH_TABLE *);
UINT_32 probe_pv_table(BOARD_STRUCT*);
int get_pv_line(const int ,BOARD_STRUCT* );
void store_hash_entry(BOARD_STRUCT * ,const UINT_32 ,int,const int ,const int);
bool probe_hash_table(BOARD_STRUCT* ,UINT_32* ,int* ,int ,int,int );
#endif // PV_TABLE_H
