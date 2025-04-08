#include "ChessEngine.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

static Piece board[8][8];
static bool isInitialized = false;

static int selectedRow = -1;
static int selectedCol = -1;
static int inAir = 0;

const char* getChessPieceName(Piece piece) {
    switch (piece.type) {
        case PAWN:   return "Pawn";
        case ROOK:   return "Rook";
        case KNIGHT: return "Knight";
        case BISHOP: return "Bishop";
        case QUEEN:  return "Queen";
        case KING:   return "King";
        case EMPTY:  return "Empty";
        default:     return "Unknown";
    }
}

void getChessPieceSymbol(Piece p, char *symbol) {
    if (p.type == EMPTY) {
        strcpy(symbol, ". ");
    } else if (p.type == PAWN) {
        strcpy(symbol, (p.color == COLOR_WHITE ? "♟ " : "♙ "));
    } else if (p.type == ROOK) {
        strcpy(symbol, (p.color == COLOR_WHITE ? "♜ " : "♖ "));
    } else if (p.type == KNIGHT) {
        strcpy(symbol, (p.color == COLOR_WHITE ? "♞ " : "♘ "));
    } else if (p.type == BISHOP) {
        strcpy(symbol, (p.color == COLOR_WHITE ? "♝ " : "♗ "));
    } else if (p.type == QUEEN) {
        strcpy(symbol, (p.color == COLOR_WHITE ? "♛ " : "♕ "));
    } else if (p.type == KING) {
        strcpy(symbol, (p.color == COLOR_WHITE ? "♚ " : "♔ "));
    }
}

void printChessBoard() {
    int possible[8][8] = {0};
    bool pieceSelected = (selectedRow != -1 && selectedCol != -1);

    if (pieceSelected) {
        ChessEngine_getPossibleMoves(selectedRow, selectedCol, possible);
    }

    char symbol[5];
    printf("\n  A  B  C  D  E  F  G  H\n");
    for (int r = 7; r >= 0; r--) {
        printf("%d ", r + 1);
        for (int c = 0; c < 8; c++) {
            if (pieceSelected && possible[r][c]) {
                if (board[r][c].type == EMPTY) {
                    printf("⬤  ");
                } else if (board[r][c].color != board[selectedRow][selectedCol].color) {
                    printf("✹  ");
                } else {
                    getChessPieceSymbol(board[r][c], symbol);
                    printf("%s ", symbol);
                }
            } else {
                getChessPieceSymbol(board[r][c], symbol);
                printf("%s ", symbol);
            }
        }
        printf("\n");
    }
    printf("\n");
}

static Color playerToColor(Player player) {
    return (player == PLAYER_WHITE) ? COLOR_WHITE : COLOR_BLACK;
}

static void ChessEngine_initializeBoard() {

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            board[r][c].type = EMPTY;
            board[r][c].color = NONE;
            board[r][c].moved = false;
        }
    }

    for (int c = 0; c < 8; ++c) {
        board[1][c].type = PAWN;
        board[1][c].color = COLOR_WHITE;

        board[6][c].type = PAWN;
        board[6][c].color = COLOR_BLACK;
    }

    PieceType layout[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };

    for (int c = 0; c < 8; ++c) {
        board[0][c].type = layout[c];
        board[0][c].color = COLOR_WHITE;

        board[7][c].type = layout[c];
        board[7][c].color = COLOR_BLACK;
    }
}

bool ChessEngine_isPieceInAir(void) {
    return inAir != 0;
}

void ChessEngine_getPickupSquare(int *row, int *col) {
    *row = selectedRow;
    *col = selectedCol;
}

bool ChessEngine_isGameOver() {
    assert(isInitialized);

    bool whiteKingExists = false;
    bool blackKingExists = false;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (board[r][c].type == KING) {
                if (board[r][c].color == COLOR_WHITE) {
                    whiteKingExists = true;
                } else if (board[r][c].color == COLOR_BLACK) {
                    blackKingExists = true;
                }
            }
        }
    }

    return !(whiteKingExists && blackKingExists);
}

void ChessEngine_getState(Piece dest[8][8]) {
    assert(isInitialized);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            dest[r][c] = board[r][c];
        }
    }
}

void ChessEngine_getPossibleMoves(int row, int col, int (*possibleMoves)[8]) {
    assert(isInitialized);

    Piece piece = board[row][col];
    if (piece.type == EMPTY) return;

    // Clear the possibleMoves board
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            possibleMoves[i][j] = 0;

    if (piece.type == PAWN) {
        if (piece.color == COLOR_BLACK) {
            // Move forward
            if (row - 1 >= 0 && board[row - 1][col].type == EMPTY) {
                possibleMoves[row - 1][col] = 1;
                if (row == 6 && board[row - 2][col].type == EMPTY)
                    possibleMoves[row - 2][col] = 1;
            }
            // Diagonal captures
            if (row - 1 >= 0 && col - 1 >= 0 &&
                board[row - 1][col - 1].type != EMPTY &&
                board[row - 1][col - 1].color == COLOR_WHITE)
                possibleMoves[row - 1][col - 1] = 1;

            if (row - 1 >= 0 && col + 1 < 8 &&
                board[row - 1][col + 1].type != EMPTY &&
                board[row - 1][col + 1].color == COLOR_WHITE)
                possibleMoves[row - 1][col + 1] = 1;

        } else if (piece.color == COLOR_WHITE) {
            // Move forward
            if (row + 1 < 8 && board[row + 1][col].type == EMPTY) {
                possibleMoves[row + 1][col] = 1;
                if (row == 1 && board[row + 2][col].type == EMPTY)
                    possibleMoves[row + 2][col] = 1;
            }
            // Diagonal captures
            if (row + 1 < 8 && col - 1 >= 0 &&
                board[row + 1][col - 1].type != EMPTY &&
                board[row + 1][col - 1].color == COLOR_BLACK)
                possibleMoves[row + 1][col - 1] = 1;

            if (row + 1 < 8 && col + 1 < 8 &&
                board[row + 1][col + 1].type != EMPTY &&
                board[row + 1][col + 1].color == COLOR_BLACK)
                possibleMoves[row + 1][col + 1] = 1;
        }
    }

    else if (piece.type == KNIGHT) {
        int moves[8][2] = {
            {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
            {1, -2},  {1, 2},  {2, -1},  {2, 1}
        };
        for (int i = 0; i < 8; i++) {
            int nr = row + moves[i][0], nc = col + moves[i][1];
            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY || board[nr][nc].color != piece.color)
                    possibleMoves[nr][nc] = 1;
            }
        }
    }

    if (piece.type == ROOK || piece.type == QUEEN) {
        int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
        for (int i = 0; i < 4; i++) {
            int dr = directions[i][0], dc = directions[i][1];
            int nr = row + dr, nc = col + dc;
            while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY) {
                    possibleMoves[nr][nc] = 1;
                } else {
                    if (board[nr][nc].color != piece.color)
                        possibleMoves[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }

    if (piece.type == BISHOP || piece.type == QUEEN) {
        int directions[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
        for (int i = 0; i < 4; i++) {
            int dr = directions[i][0], dc = directions[i][1];
            int nr = row + dr, nc = col + dc;
            while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY) {
                    possibleMoves[nr][nc] = 1;
                } else {
                    if (board[nr][nc].color != piece.color)
                        possibleMoves[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }

    if (piece.type == KING) {
        int moves[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };
        for (int i = 0; i < 8; i++) {
            int nr = row + moves[i][0], nc = col + moves[i][1];
            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY || board[nr][nc].color != piece.color)
                    possibleMoves[nr][nc] = 1;
            }
        }
    }
}

MoveResult ChessEngine_ProcessMove(int row, int col, Player turn) {
    assert(isInitialized);

    Piece current = board[row][col];

    // --- PICKUP ---
    if (!inAir) {
        if (current.type == EMPTY) {
            printf("No piece to pick at %d%c\n", row + 1, 'A' + col);
            return MOVE_PICKUP_INVALID;
        }

        if (current.color != playerToColor(turn)) {
            printf("Cannot pick opponent's piece!\n");
            return MOVE_PICKUP_INVALID;
        }

        selectedRow = row;
        selectedCol = col;
        inAir = 1;

        printf("Picked up %s at %d%c\n", getChessPieceName(current), row + 1, 'A' + col);
        printChessBoard();
        return MOVE_PICKUP_VALID;
    }

    // --- DROPOFF ---
    int possible[8][8] = {0};
    ChessEngine_getPossibleMoves(selectedRow, selectedCol, possible);

    if (!possible[row][col]) {
        if (row == selectedRow && col == selectedCol) {
            printf("Move cancelled.\n");
            inAir = 0;
            selectedRow = -1;
            selectedCol = -1;
            return MOVE_CANCELLED;
        } else {
            printf("Invalid move to %d%c\n", row + 1, 'A' + col);
            inAir = 0;
            selectedRow = -1;
            selectedCol = -1;
            return MOVE_DROP_INVALID;
        }
    }

    // Prevent capturing your own piece
    if (board[row][col].type != EMPTY && board[row][col].color == playerToColor(turn)) {
        printf("Cannot capture your own piece at %d%c\n", row + 1, 'A' + col);
        inAir = 0;
        selectedRow = -1;
        selectedCol = -1;
        return MOVE_DROP_INVALID;
    }

    // Determine if this is a capture
    bool isCapture = (board[row][col].type != EMPTY);

    if (isCapture) {
        printf("Captured %s at %d%c\n", getChessPieceName(board[row][col]), row + 1, 'A' + col);

        if (board[row][col].type == KING) {
            printf("%s's King captured. Game Over!\n", board[row][col].color == COLOR_WHITE ? "White" : "Black");
            inAir = 0;
            selectedRow = -1;
            selectedCol = -1;
            return MOVE_GAME_OVER; // Optional if you have such enum
        }
    }

    // Move the piece
    board[row][col] = board[selectedRow][selectedCol];
    board[selectedRow][selectedCol].type = EMPTY;
    board[selectedRow][selectedCol].color = NONE;

    printf("Moved to %d%c\n", row + 1, 'A' + col);
    printChessBoard();

    // Pawn promotion
    if (board[row][col].type == PAWN) {
        if ((board[row][col].color == COLOR_WHITE && row == 7) ||
            (board[row][col].color == COLOR_BLACK && row == 0)) {
            board[row][col].type = QUEEN;
            printf("Pawn promoted to Queen at %d%c\n", row + 1, 'A' + col);
            inAir = 0;
            selectedRow = -1;
            selectedCol = -1;
            return MOVE_PROMOTION;
        }
    }

    inAir = 0;
    selectedRow = -1;
    selectedCol = -1;

    return isCapture ? MOVE_CAPTURE : MOVE_DROP_VALID;
}



void ChessEngine_init() {
    
    ChessEngine_initializeBoard();
    isInitialized = true;
}

void ChessEngine_cleanup() {

    assert(isInitialized);
    isInitialized = false;
}