#include <stdio.h>
#include "ChessEngine.h"
#include "GameController.h"

void Game_engine_manager_init();

void Game_engine_manager_cleanup();

void Game_engine_manager_processBoardState(Piece boardState[8][8], int totalMoves, char *from, char *to, bool *isCheck, bool *isCheckMate);