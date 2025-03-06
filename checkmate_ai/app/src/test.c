#include "chess_board.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    uint8_t boardState[64];

    //set all boardState to 0
    for (int i = 0; i < 64; i++) {
        boardState[i] = 0;
    }

    chessBoard_init(); // Initialize MCP23017 expanders

    while (1) {
        chessBoard_getBoardState(boardState);

        // Print board state
        printf("Board State: \n");
        for (int i = 0; i < 64; i++) {
            printf("%d ", boardState[i]);
            if ((i + 1) % 8 == 0) printf("\n");
        }

        usleep(500000); // Read every 500ms
    }

    return 0;
}