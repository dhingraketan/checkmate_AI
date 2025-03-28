#include "hal/chessHelper.h"

// Define global variables.
Piece board[8][8];
int possible[8][8];
int pieceSelected = 0;
int selectedRow = -1, selectedCol = -1;
Color currentTurn = BLACK;

// Initialize board with standard chess starting positions.
void initializeBoard() {
    // Set all squares to EMPTY.
    for (int r = 0; r < 8; r++){
        for (int c = 0; c < 8; c++){
            board[r][c].type = EMPTY;
            board[r][c].color = NONE;
            board[r][c].moved = 0;
            board[r][c].side = NONE_SIDE; // Default side.
        }
    }
    // Standard setup:
    // Black pieces (placed on rows 0 and 1)
    // Row 0.
    board[0][0].type = ROOK;   board[0][0].color = BLACK; board[0][0].side = QUEEN_SIDE;
    board[0][1].type = KNIGHT; board[0][1].color = BLACK; board[0][1].side = QUEEN_SIDE;
    board[0][2].type = BISHOP; board[0][2].color = BLACK; board[0][2].side = QUEEN_SIDE;
    board[0][3].type = QUEEN;  board[0][3].color = BLACK; board[0][3].side = NONE_SIDE;
    board[0][4].type = KING;   board[0][4].color = BLACK; board[0][4].side = NONE_SIDE;
    board[0][5].type = BISHOP; board[0][5].color = BLACK; board[0][5].side = KING_SIDE;
    board[0][6].type = KNIGHT; board[0][6].color = BLACK; board[0][6].side = KING_SIDE;
    board[0][7].type = ROOK;   board[0][7].color = BLACK; board[0][7].side = KING_SIDE;
    for (int c = 0; c < 8; c++){
        board[1][c].type = PAWN;
        board[1][c].color = BLACK;
        board[1][c].side = NONE_SIDE;
    }
    // White pieces (placed on rows 6 and 7)
    for (int c = 0; c < 8; c++){
        board[6][c].type = PAWN;
        board[6][c].color = WHITE;
        board[6][c].side = NONE_SIDE;
    }
    board[7][0].type = ROOK;   board[7][0].color = WHITE; board[7][0].side = QUEEN_SIDE;
    board[7][1].type = KNIGHT; board[7][1].color = WHITE; board[7][1].side = QUEEN_SIDE;
    board[7][2].type = BISHOP; board[7][2].color = WHITE; board[7][2].side = QUEEN_SIDE;
    board[7][3].type = QUEEN;  board[7][3].color = WHITE; board[7][3].side = NONE_SIDE;
    board[7][4].type = KING;   board[7][4].color = WHITE; board[7][4].side = NONE_SIDE;
    board[7][5].type = BISHOP; board[7][5].color = WHITE; board[7][5].side = KING_SIDE;
    board[7][6].type = KNIGHT; board[7][6].color = WHITE; board[7][6].side = KING_SIDE;
    board[7][7].type = ROOK;   board[7][7].color = WHITE; board[7][7].side = KING_SIDE;
}

// Print the board state and possible moves.
void printBoard() {
    char symbol[5];
    // Print column header.
    printf("  A  B  C  D  E  F  G  H\n");
    for (int r = 0; r < 8; r++){
        printf("%d ", r + 1);
        for (int c = 0; c < 8; c++){
            // If a piece is selected and this square is a valid move, show a marker.
            if (pieceSelected && possible[r][c]) {
                if (board[r][c].type != EMPTY && board[r][c].color != board[selectedRow][selectedCol].color)
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
    if (p.type == EMPTY) {
        strcpy(symbol, ". ");
    } else if (p.type == PAWN) {
        strcpy(symbol, (p.color == WHITE ? "♙ " : "♟ "));
    } else if (p.type == ROOK) {
        strcpy(symbol, (p.color == WHITE ? "♖ " : "♜ "));
    } else if (p.type == KNIGHT) {
        strcpy(symbol, (p.color == WHITE ? "♘ " : "♞ "));
    } else if (p.type == BISHOP) {
        strcpy(symbol, (p.color == WHITE ? "♗ " : "♝ "));
    } else if (p.type == QUEEN) {
        strcpy(symbol, (p.color == WHITE ? "♕ " : "♛ "));
    } else if (p.type == KING) {
        strcpy(symbol, (p.color == WHITE ? "♔ " : "♚ "));
    }
}

// Parse an input coordinate (like "7A") into row and column indices.
int parseCoordinate(const char *input, int *row, int *col) {
    int r;
    char colChar;
    // Expect input format: a number (row) followed by a letter (column).
    if (sscanf(input, "%d%c", &r, &colChar) != 2)
        return 0;
    colChar = toupper(colChar);
    if (r < 1 || r > 8 || colChar < 'A' || colChar > 'H')
        return 0;
    *row = r - 1;
    *col = colChar - 'A';
    return 1;
}

// Calculate valid moves for the piece at board[r][c].
// (Basic moves for each piece type; special moves are omitted.)
void getPossibleMoves(int r, int c) {
    Piece piece = board[r][c];
    if (piece.type == EMPTY)
        return;
    
    // Pawn moves.
    if (piece.type == PAWN) {
        if (piece.color == WHITE) {
            if (r - 1 >= 0 && board[r - 1][c].type == EMPTY) {
                possible[r - 1][c] = 1;
                if (r == 6 && board[r - 2][c].type == EMPTY)
                    possible[r - 2][c] = 1;
            }
            // Diagonal captures.
            if (r - 1 >= 0 && c - 1 >= 0 &&
                board[r - 1][c - 1].type != EMPTY &&
                board[r - 1][c - 1].color == BLACK)
                possible[r - 1][c - 1] = 1;
            if (r - 1 >= 0 && c + 1 < 8 &&
                board[r - 1][c + 1].type != EMPTY &&
                board[r - 1][c + 1].color == BLACK)
                possible[r - 1][c + 1] = 1;
        } else { // BLACK pawn moves.
            if (r + 1 < 8 && board[r + 1][c].type == EMPTY) {
                possible[r + 1][c] = 1;
                if (r == 1 && board[r + 2][c].type == EMPTY)
                    possible[r + 2][c] = 1;
            }
            if (r + 1 < 8 && c - 1 >= 0 &&
                board[r + 1][c - 1].type != EMPTY &&
                board[r + 1][c - 1].color == WHITE)
                possible[r + 1][c - 1] = 1;
            if (r + 1 < 8 && c + 1 < 8 &&
                board[r + 1][c + 1].type != EMPTY &&
                board[r + 1][c + 1].color == WHITE)
                possible[r + 1][c + 1] = 1;
        }
    }
    // Knight moves.
    else if (piece.type == KNIGHT) {
        int moves[8][2] = {
            {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
            {1, -2},  {1, 2},  {2, -1},  {2, 1}
        };
        for (int i = 0; i < 8; i++) {
            int nr = r + moves[i][0], nc = c + moves[i][1];
            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY || board[nr][nc].color != piece.color)
                    possible[nr][nc] = 1;
            }
        }
    }
    // Rook moves.
    else if (piece.type == ROOK) {
        int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
        for (int i = 0; i < 4; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if (board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // Bishop moves.
    else if (piece.type == BISHOP) {
        int directions[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
        for (int i = 0; i < 4; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if (board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // Queen moves: combination of rook and bishop moves.
    else if (piece.type == QUEEN) {
        int directions[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };
        for (int i = 0; i < 8; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if (board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // King moves.
    else if (piece.type == KING) {
        int moves[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };
        for (int i = 0; i < 8; i++) {
            int nr = r + moves[i][0], nc = c + moves[i][1];
            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY || board[nr][nc].color != piece.color)
                    possible[nr][nc] = 1;
            }
        }
        // Note: Castling is not implemented in this simple version.
    }
}

// Return a string name for the piece type.
const char* getPieceName(Piece p) {
    switch (p.type) {
        case PAWN:   return "Pawn";
        case ROOK:   return "Rook";
        case KNIGHT: return "Knight";
        case BISHOP: return "Bishop";
        case QUEEN:  return "Queen";
        case KING:   return "King";
        default:     return "Empty";
    }
}
