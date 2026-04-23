#ifndef UCI_PROTOCOL_H
#define UCI_PROTOCOL_H

#include <string>
#include <vector>
#include <iostream>
#include "IO.h"
#include "Fen.h"
#include "Movegen.h"
#include "ChessBoard.h"
#include "Search.h"
#include "Timer.h"


#include<thread>
 void uci_loop(BOARD_STRUCT*,SEARCH_INFORMATION*);
 bool parse_go(std::string,BOARD_STRUCT* ,SEARCH_INFORMATION* );
bool parse_position(std::string line,BOARD_STRUCT*);
#endif // UCI_PROTOCOL_H
