#ifndef IO_H
#define IO_H

#include "definitions.h"
#include <string>
#include <iostream>
#include "Validate.h"
#include <algorithm>

//string conversions and input output handling


void print_move(UINT_32);
void print_square_to_file_ranks(int);
void print_movelist(MOVELIST* );
std::string convert_move_to_string(UINT_32);
std::string convert_square_to_file_ranks(int );
UINT_32 find_move_string(std::string ,const MOVELIST *);
int convert_file_ranks_to_square(std::string );
void split_string_by_delimiter(std::string ,std::string,std::vector<std::string>&);
int char_to_int(char);
int convert_str_to_int(std::string);
void print_chessboard(int []);
void print_board_numbers(int []);
void print_lists(BOARD_STRUCT* );
#endif // IO_H
