#include "chess_board.h"
#include "port_extender.h"

#include <stdbool.h>

static bool is_initialized = false;

void chessBoard_init() {
    portExtender_init_and_config();
    is_initialized = true;
}

void chessBoard_getBoardState(uint8_t boardState[8][8]) {
    
    if (!is_initialized) {
        return;
    }

    portExtender_readAllPins(boardState);
}

void chessBoard_cleanup() {
    is_initialized = false;
}