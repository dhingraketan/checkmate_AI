#ifndef _CHESS_HELPER_H
#define _CHESS_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>

// Define piece types and colors.
typedef enum { EMPTY, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING } PieceType;
typedef enum { NONE, WHITE, BLACK } Color;

// New enum for side classification.
typedef enum { NONE_SIDE, KING_SIDE, QUEEN_SIDE } Side;

typedef struct {
    PieceType type;
    Color color;
    int moved;  // For future use (pawn first move, castling, etc.)
    Side side;  // Indicates if the piece is king-side or queen-side.
} Piece;


// Function prototypes.
void initializeBoard();
void printBoard();
void getPieceSymbol(Piece p, char *symbol);
int parseCoordinate(const char *input, int *row, int *col);
void getPossibleMoves(int r, int c);
const char* getPieceName(Piece p);


void *chessGameThread(void *arg);

int validateInput(const char *input);

void processInput(const char *input);

void processPieceSelection(const char *input);

void processDestination(const char *input);

void copyBoardState(Piece dest[8][8]); //for board state

char getCurrentTurnString();

void toggleCurrentTurn();

void copyPossibleMoves(int dest[8][8]);

void chessHelper_cleanup();

bool chessHelper_getIsValidMove();

#endif
