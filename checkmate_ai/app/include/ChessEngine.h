#ifndef _CHESS_ENGINE_H_
#define _CHESS_ENGINE_H_

#include <stdbool.h>

typedef enum { EMPTY, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING } PieceType;
typedef enum { NONE, COLOR_WHITE, COLOR_BLACK } Color;

typedef struct {
    PieceType type;
    Color color;
    bool moved;
} Piece;

typedef enum {
    PLAYER_WHITE,
    PLAYER_BLACK

} Player;

typedef enum {
    MOVE_NONE,
    MOVE_PICKUP_VALID,
    MOVE_PICKUP_INVALID,
    MOVE_DROP_INVALID,
    MOVE_DROP_VALID,
    MOVE_CAPTURE,
    MOVE_PROMOTION,
    MOVE_GAME_OVER,
    MOVE_CANCELLED
} MoveResult;

void ChessEngine_init();
void ChessEngine_cleanup();
MoveResult ChessEngine_ProcessMove(int row, int col, Player turn);
void ChessEngine_getPossibleMoves(int row, int col, int (*possibleMoves)[8]);
void ChessEngine_getState(Piece dest[8][8]);
bool ChessEngine_isPieceInAir();
void ChessEngine_getPickupSquare(int *row, int *col);
bool ChessEngine_isGameOver();

#endif