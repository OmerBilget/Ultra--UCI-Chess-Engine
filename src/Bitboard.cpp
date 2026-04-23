#include "Bitboard.h"




//makes first bit with value of one zero and returns index of that
int pop_bit(UINT_64 *bb) {
  UINT_64 b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

//returns non zero bit count
//1001 9  returns 2
int count_bits(UINT_64 b) {
  int r;
  for(r = 0; b; r++, b &= b - 1);
  return r;
}
//get index of lsb
int lsb64(UINT_64 bb) {
  UINT_64 b = bb ^ (bb - 1);
  unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
  return BitTable[(fold * 0x783a9b23) >> 26];
}

//get index of msb
int msb64 (UINT_64 value)
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return tab64[((UINT_64)((value - (value >> 1))*0x07EDD5E59A4E28C2)) >> 58];
}


void print_bitboard(UINT_64 bitboard){
    UINT_64 shift=1ULL;
    for(int i=0;i<64;i++){
        if((shift<<i)&bitboard){
            printf("X ");
        }else{
            printf(". ");
        }
        if(((i+1)%8)==0){
            printf("\n");
        }
    }
}

