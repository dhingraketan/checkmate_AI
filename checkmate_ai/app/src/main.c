#include "hal/chessHelper.h"

int main() {
    char input[100];
    //char temp[100];

    // Initialize the board with standard starting positions.
    initializeBoard();

    while (1) {
        // Display whose turn it is.
        printf("\n%s Turn\n", (currentTurn == WHITE ? "WHITE" : "BLACK"));
        // Print the board.
        printBoard();
        
        // Display piece selection status.
        if (pieceSelected)
            printf("inAir [%d%c]\n", selectedRow + 1, 'A' + selectedCol);
        else
            printf("inAir []\n");

        // Prompt for input.
        printf("INPUT: ");
        if (!fgets(input, sizeof(input), stdin))
            break;
        // Remove trailing newline.
        input[strcspn(input, "\n")] = 0;
        
        // Check if user mistakenly entered two coordinates.
        if (strchr(input, ',') != NULL) {
            printf("Error: Cannot pick two pieces at once!\n");
            continue;
        }
        
        // If no piece is selected, treat input as piece selection.
        if (!pieceSelected) {
            int row, col;
            if (!parseCoordinate(input, &row, &col)) {
                printf("Invalid input format. Try again.\n");
                continue;
            }
            // Verify the square contains a piece of the current turn.
            if (board[row][col].type == EMPTY || board[row][col].color != currentTurn) {
                printf("Invalid selection: No %s piece at that square.\n",
                       (currentTurn == WHITE ? "WHITE" : "BLACK"));
                continue;
            }
            pieceSelected = 1;
            selectedRow = row;
            selectedCol = col;
            memset(possible, 0, sizeof(possible));
            getPossibleMoves(row, col);
        }
        // Otherwise, treat input as the destination.
        else {
            int destRow, destCol;
            if (!parseCoordinate(input, &destRow, &destCol)) {
                printf("Invalid input format. Try again.\n");
                continue;
            }
            if (!possible[destRow][destCol]) {
                // Allow deselection if the same square is chosen.
                if (destRow == selectedRow && destCol == selectedCol) {
                    pieceSelected = 0;
                    memset(possible, 0, sizeof(possible));
                    continue;
                }
                printf("Invalid move, try again.\n");
                continue;
            }
            // Handle captures.
            if (board[destRow][destCol].type != EMPTY) {
                if (board[destRow][destCol].type == KING) {
                    printf("Captured %s %s at %d%c.\n", 
                           (board[destRow][destCol].color == WHITE ? "White" : "Black"),
                           getPieceName(board[destRow][destCol]),
                           destRow + 1, 'A' + destCol);
                    printf("Game over: %s King has been killed.\n", 
                           (board[destRow][destCol].color == WHITE ? "White" : "Black"));
                    exit(0);
                } else {
                    printf("Captured %s %s at %d%c.\n", 
                           (board[destRow][destCol].color == WHITE ? "White" : "Black"),
                           getPieceName(board[destRow][destCol]),
                           destRow + 1, 'A' + destCol);
                }
            }
            // Move the piece.
            board[destRow][destCol] = board[selectedRow][selectedCol];
            board[selectedRow][selectedCol].type = EMPTY;
            board[selectedRow][selectedCol].color = NONE;
            board[selectedRow][selectedCol].moved = 0;
            
            // Check for pawn promotion.
            if (board[destRow][destCol].type == PAWN) {
                if ((board[destRow][destCol].color == WHITE && destRow == 0) ||
                    (board[destRow][destCol].color == BLACK && destRow == 7)) {
                    board[destRow][destCol].type = QUEEN;
                    printf("Pawn promoted to Queen at %d%c.\n", destRow + 1, 'A' + destCol);
                }
            }
            
            // Clear selection and switch turn.
            pieceSelected = 0;
            memset(possible, 0, sizeof(possible));
            currentTurn = (currentTurn == WHITE ? BLACK : WHITE);
        }
    }
    return 0;
}
