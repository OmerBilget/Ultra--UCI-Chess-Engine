#ifndef FEN_H
#define FEN_H


#include  "definitions.h"
#include <string>
#include <vector>
#include  "IO.h"
#include "ChessBoard.h"
#include "HashKey.h"
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
bool is_digit(char );
bool parse_fen_string(std::string,BOARD_STRUCT * chessboard);


#endif // FEN_H
