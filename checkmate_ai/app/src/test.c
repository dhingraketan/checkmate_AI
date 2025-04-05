#include "BoardReader.h"
#include <stdio.h>
#include <unistd.h>

// int main() {
//     printf("=== Smart Chess Board Test ===\n");

//     // Initialize board reader (will wait until board is correctly set up)
//     boardReader_init();

//     printf("System initialized. Waiting for player moves...\n");

//     while (1) {
//         int fromRank, fromFile;
//         int toRank, toFile;

//         // Detect piece pickup
//         if (boardReader_detectPickup(&fromRank, &fromFile)) {
//             printf("Piece picked up from: %c%d\n", 'a' + fromFile, fromRank + 1);

//             // Show waiting indicator
//             printf("Waiting for piece drop...\n");

//             // Detect drop
//             while (!boardReader_detectDrop(&toRank, &toFile)) {
//                 usleep(10000); // 10 ms delay
//             }

//             printf("Piece dropped at: %c%d\n", 'a' + toFile, toRank + 1);
//             printf("=> Move: %c%d to %c%d\n\n",
//                    'a' + fromFile, fromRank + 1,
//                    'a' + toFile, toRank + 1);
//         }

//         usleep(10000); // Polling delay
//     }

//     boardReader_cleanup();
//     return 0;
// }

// #include "chess_board.h"
// #include <stdio.h>
// #include <unistd.h>

// void print_Board(uint8_t state[8][8]) {
//     printf("   a   b   c   d   e   f   g   h\n");
//     printf("  +---+---+---+---+---+---+---+---+\n");

//     for (int rank = 7; rank >= 0; rank--) {  // Print from rank 8 to 1
//         printf("%d |", rank + 1);
//         for (int file = 0; file < 8; file++) {
//             printf(" %c |", state[rank][file] ? 'X' : ' ');
//         }
//         printf(" %d\n", rank + 1);
//         printf("  +---+---+---+---+---+---+---+---+\n");
//     }

//     printf("   a   b   c   d   e   f   g   h\n");
// }

// int main() {
    
//     uint8_t boardState[8][8];
//     chessBoard_init();
    
//     // set all boardState to 0
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             boardState[i][j] = 0;
//         }
//     }

//     while (1) {
//         chessBoard_getBoardState(boardState);
//         print_Board(boardState);
//         usleep(10000);
//     }
    
//     return 0;
// }

// #include "GameController.h"
// #include "LCDPrinter.h"

// int main(){
//     lcd_printer_init();
//     GameController_init();
// }
