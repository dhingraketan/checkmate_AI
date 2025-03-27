#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define piece types and colors.
typedef enum { EMPTY, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING } PieceType;
typedef enum { NONE, WHITE, BLACK } Color;

typedef struct {
    PieceType type;
    Color color;
    int moved; // For future use (pawn first move, castling, etc.)
} Piece;

// Global board: board[row][col] where row:0..7 (maps to 1..8), col:0..7 (maps to A..H)
Piece board[8][8];

// Array to mark possible moves (1 = valid move, 0 = not)
int possible[8][8];

// Variables for selection state
int pieceSelected = 0;  // 0: no piece picked, 1: piece in air
int selectedRow = -1, selectedCol = -1;

// Global variable for turn management; starting with BLACK as in your sample.
Color currentTurn = BLACK;

// Function prototypes
void initializeBoard();
void printBoard();
void getPieceSymbol(Piece p, char *symbol);
int parseCoordinate(const char *input, int *row, int *col);
void getPossibleMoves(int r, int c);
const char* getPieceName(Piece p);

int main() {
    char input[100];

    // Initialize the board with standard chess starting positions.
    initializeBoard();

    while(1) {
        // Display whose turn it is.
        printf("\n%s Turn\n", (currentTurn == WHITE ? "WHITE" : "BLACK"));
        // Print the board with any possible-move markers.
        printBoard();
        
        // Show inAir status.
        if(pieceSelected)
            printf("inAir [%d%c]\n", selectedRow + 1, 'A' + selectedCol);
        else
            printf("inAir []\n");

        // Prompt for input.
        printf("INPUT: ");
        if(!fgets(input, sizeof(input), stdin)) {
            break;
        }
        // Remove trailing newline.
        input[strcspn(input, "\n")] = 0;
        
        // Check if user mistakenly entered two coordinates (e.g. "2B, 4B")
        if(strchr(input, ',') != NULL) {
            printf("Error: Cannot pick two pieces at once!\n");
            continue;
        }
        
        // If no piece is selected yet, then input is the piece selection.
        if(!pieceSelected) {
            int row, col;
            if(!parseCoordinate(input, &row, &col)) {
                printf("Invalid input format. Try again.\n");
                continue;
            }
            // Check that the square contains a piece of the current turn.
            if(board[row][col].type == EMPTY || board[row][col].color != currentTurn) {
                printf("Invalid selection: No %s piece at that square.\n",
                       (currentTurn == WHITE ? "WHITE" : "BLACK"));
                continue;
            }
            // Set the piece "inAir".
            pieceSelected = 1;
            selectedRow = row;
            selectedCol = col;
            // Calculate possible moves.
            memset(possible, 0, sizeof(possible));
            getPossibleMoves(row, col);
        }
        // Otherwise, a piece is already inAir and input is the destination.
        else {
            int destRow, destCol;
            if(!parseCoordinate(input, &destRow, &destCol)) {
                printf("Invalid input format. Try again.\n");
                continue;
            }
            // If destination is not a valid move for the selected piece.
            if(!possible[destRow][destCol]) {
                // Allow deselection if user inputs the same coordinate.
                if(destRow == selectedRow && destCol == selectedCol) {
                    pieceSelected = 0;
                    memset(possible, 0, sizeof(possible));
                    continue;
                }
                printf("Invalid move, try again.\n");
                continue;
            }
            // If destination contains an opponent's piece, announce capture.
            if(board[destRow][destCol].type != EMPTY) {
                if(board[destRow][destCol].type == KING) {
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
            // Empty the source square.
            board[selectedRow][selectedCol].type = EMPTY;
            board[selectedRow][selectedCol].color = NONE;
            board[selectedRow][selectedCol].moved = 0;
            
            // Check for pawn promotion:
            // For WHITE: pawn promoted if it reaches row 0.
            // For BLACK: pawn promoted if it reaches row 7.
            if(board[destRow][destCol].type == PAWN) {
                if((board[destRow][destCol].color == WHITE && destRow == 0) ||
                   (board[destRow][destCol].color == BLACK && destRow == 7)) {
                    board[destRow][destCol].type = QUEEN;
                    printf("Pawn promoted to Queen at %d%c.\n", destRow + 1, 'A' + destCol);
                }
            }
            
            // Clear selection.
            pieceSelected = 0;
            memset(possible, 0, sizeof(possible));
            // Switch turn.
            currentTurn = (currentTurn == WHITE ? BLACK : WHITE);
        }
    }
    return 0;
}

// Initialize board with standard chess starting positions.
void initializeBoard() {
    // Set all squares to EMPTY.
    for(int r = 0; r < 8; r++){
        for(int c = 0; c < 8; c++){
            board[r][c].type = EMPTY;
            board[r][c].color = NONE;
            board[r][c].moved = 0;
        }
    }
    // Standard setup:
    // Black pieces (we place them on rows 0 and 1)
    board[0][0].type = ROOK;   board[0][0].color = BLACK;
    board[0][1].type = KNIGHT; board[0][1].color = BLACK;
    board[0][2].type = BISHOP; board[0][2].color = BLACK;
    board[0][3].type = QUEEN;  board[0][3].color = BLACK;
    board[0][4].type = KING;   board[0][4].color = BLACK;
    board[0][5].type = BISHOP; board[0][5].color = BLACK;
    board[0][6].type = KNIGHT; board[0][6].color = BLACK;
    board[0][7].type = ROOK;   board[0][7].color = BLACK;
    for(int c = 0; c < 8; c++){
        board[1][c].type = PAWN;
        board[1][c].color = BLACK;
    }
    // White pieces (placed on rows 6 and 7)
    for(int c = 0; c < 8; c++){
        board[6][c].type = PAWN;
        board[6][c].color = WHITE;
    }
    board[7][0].type = ROOK;   board[7][0].color = WHITE;
    board[7][1].type = KNIGHT; board[7][1].color = WHITE;
    board[7][2].type = BISHOP; board[7][2].color = WHITE;
    board[7][3].type = QUEEN;  board[7][3].color = WHITE;
    board[7][4].type = KING;   board[7][4].color = WHITE;
    board[7][5].type = BISHOP; board[7][5].color = WHITE;
    board[7][6].type = KNIGHT; board[7][6].color = WHITE;
    board[7][7].type = ROOK;   board[7][7].color = WHITE;
}

// Print the board state and possible moves.
void printBoard() {
    char symbol[5];
    // Print column header.
    printf("  A  B  C  D  E  F  G  H\n");
    for(int r = 0; r < 8; r++){
        printf("%d ", r + 1);
        for(int c = 0; c < 8; c++){
            // If a piece is selected and this square is a valid move, show marker.
            if(pieceSelected && possible[r][c]) {
                if(board[r][c].type != EMPTY && board[r][c].color != board[selectedRow][selectedCol].color)
                    printf("✹  ");
                else
                    printf("⬤  ");
            } else {
                getPieceSymbol(board[r][c], symbol);
                printf("%s ", symbol);
            }
        }
        printf("\n");
    }
}

// Convert a piece to its display symbol.
void getPieceSymbol(Piece p, char *symbol) {
    if(p.type == EMPTY) {
        strcpy(symbol, ". ");
    } else if(p.type == PAWN) {
        strcpy(symbol, (p.color == WHITE ? "♙ " : "♟ "));
    } else if(p.type == ROOK) {
        strcpy(symbol, (p.color == WHITE ? "♖ " : "♜ "));
    } else if(p.type == KNIGHT) {
        strcpy(symbol, (p.color == WHITE ? "♘ " : "♞ "));
    } else if(p.type == BISHOP) {
        strcpy(symbol, (p.color == WHITE ? "♗ " : "♝ "));
    } else if(p.type == QUEEN) {
        strcpy(symbol, (p.color == WHITE ? "♕ " : "♛ "));
    } else if(p.type == KING) {
        strcpy(symbol, (p.color == WHITE ? "♔ " : "♚ "));
    }
}

// Parse an input coordinate (like "7A") into row and column indices.
int parseCoordinate(const char *input, int *row, int *col) {
    int r;
    char colChar;
    // Expect input format: a number (row) followed by a letter (column).
    if(sscanf(input, "%d%c", &r, &colChar) != 2)
        return 0;
    colChar = toupper(colChar);
    if(r < 1 || r > 8 || colChar < 'A' || colChar > 'H')
        return 0;
    *row = r - 1;
    *col = colChar - 'A';
    return 1;
}

// Calculate valid moves for the piece at board[r][c].

void getPossibleMoves(int r, int c) {
    Piece piece = board[r][c];
    if(piece.type == EMPTY)
        return;
    
    // Pawn moves.
    if(piece.type == PAWN) {
        if(piece.color == WHITE) {
            if(r - 1 >= 0 && board[r - 1][c].type == EMPTY) {
                possible[r - 1][c] = 1;
                if(r == 6 && board[r - 2][c].type == EMPTY)
                    possible[r - 2][c] = 1;
            }
            // Diagonal captures.
            if(r - 1 >= 0 && c - 1 >= 0 &&
               board[r - 1][c - 1].type != EMPTY &&
               board[r - 1][c - 1].color == BLACK)
                possible[r - 1][c - 1] = 1;
            if(r - 1 >= 0 && c + 1 < 8 &&
               board[r - 1][c + 1].type != EMPTY &&
               board[r - 1][c + 1].color == BLACK)
                possible[r - 1][c + 1] = 1;
        } else { // BLACK pawn moves.
            if(r + 1 < 8 && board[r + 1][c].type == EMPTY) {
                possible[r + 1][c] = 1;
                if(r == 1 && board[r + 2][c].type == EMPTY)
                    possible[r + 2][c] = 1;
            }
            if(r + 1 < 8 && c - 1 >= 0 &&
               board[r + 1][c - 1].type != EMPTY &&
               board[r + 1][c - 1].color == WHITE)
                possible[r + 1][c - 1] = 1;
            if(r + 1 < 8 && c + 1 < 8 &&
               board[r + 1][c + 1].type != EMPTY &&
               board[r + 1][c + 1].color == WHITE)
                possible[r + 1][c + 1] = 1;
        }
    }
    // Knight moves.
    else if(piece.type == KNIGHT) {
        int moves[8][2] = {
            {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
            {1, -2}, {1, 2}, {2, -1}, {2, 1}
        };
        for (int i = 0; i < 8; i++) {
            int nr = r + moves[i][0], nc = c + moves[i][1];
            if(nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if(board[nr][nc].type == EMPTY || board[nr][nc].color != piece.color)
                    possible[nr][nc] = 1;
            }
        }
    }
    // Rook moves.
    else if(piece.type == ROOK) {
        int directions[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };
        for(int i = 0; i < 4; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while(nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if(board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if(board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // Bishop moves.
    else if(piece.type == BISHOP) {
        int directions[4][2] = { {-1,-1}, {-1,1}, {1,-1}, {1,1} };
        for(int i = 0; i < 4; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while(nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if(board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if(board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // Queen moves: combine rook and bishop.
    else if(piece.type == QUEEN) {
        int directions[8][2] = {
            {-1,0}, {1,0}, {0,-1}, {0,1},
            {-1,-1}, {-1,1}, {1,-1}, {1,1}
        };
        for(int i = 0; i < 8; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while(nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if(board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if(board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // King moves.
    else if(piece.type == KING) {
        int moves[8][2] = {
            {-1,0}, {1,0}, {0,-1}, {0,1},
            {-1,-1}, {-1,1}, {1,-1}, {1,1}
        };
        for (int i = 0; i < 8; i++) {
            int nr = r + moves[i][0], nc = c + moves[i][1];
            if(nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if(board[nr][nc].type == EMPTY || board[nr][nc].color != piece.color)
                    possible[nr][nc] = 1;
            }
        }
        
    }
}

// Return a string name for the piece type.
const char* getPieceName(Piece p) {
    switch(p.type) {
        case PAWN:   return "Pawn";
        case ROOK:   return "Rook";
        case KNIGHT: return "Knight";
        case BISHOP: return "Bishop";
        case QUEEN:  return "Queen";
        case KING:   return "King";
        default:     return "Empty";
    }
}
