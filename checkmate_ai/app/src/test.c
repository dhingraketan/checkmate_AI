#include "chess_board.h"
#include <stdio.h>
#include <unistd.h>

void printBoard(uint8_t state[8][8]) {
    printf("   a   b   c   d   e   f   g   h\n");
    printf("  +---+---+---+---+---+---+---+---+\n");

    for (int rank = 7; rank >= 0; rank--) {  // Print from rank 8 to 1
        printf("%d |", rank + 1);
        for (int file = 0; file < 8; file++) {
            printf(" %c |", state[rank][file] ? 'X' : ' ');
        }
        printf(" %d\n", rank + 1);
        printf("  +---+---+---+---+---+---+---+---+\n");
    }

    printf("   a   b   c   d   e   f   g   h\n");
}

int main() {
    
    uint8_t boardState[8][8];
    chessBoard_init();
    
    // set all boardState to 0
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            boardState[i][j] = 0;
        }
    }

    while (1) {
        chessBoard_getBoardState(boardState);
        printBoard(boardState);
        usleep(10000);
    }
    
    return 0;
}