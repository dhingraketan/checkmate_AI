#include "chess_board.h"
#include "port_extender.h"

#include <stdbool.h>

static bool is_initialized = false;

void chessBoard_init() {
    portExtender_init_and_config();
    is_initialized = true;
}

void chessBoard_getBoardState(uint8_t boardState[64]) {
    if (!is_initialized) {
        return;
    }

    for (int i = 0; i < 1; i++) {
        uint8_t portData[16];
        portExtender_readAllPins(i + 1, portData);

        // Store pin states properly in boardState array
        for (int j = 0; j < 16; j++) {
            boardState[(i * 16) + j] = portData[j];
        }
    }
}

void chessBoard_cleanup() {
    is_initialized = false;
}