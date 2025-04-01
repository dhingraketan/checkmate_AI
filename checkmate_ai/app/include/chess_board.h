#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <stdint.h>

void chessBoard_init();
void chessBoard_getBoardState(uint8_t boardState[8][8]);
void chessBoard_cleanup();

#endif // CHESS_BOARD_H