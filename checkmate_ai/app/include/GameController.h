#ifndef _GAME_CONTROLLER_H_
#define _GAME_CONTROLLER_H_

#include "timer.h"
#include "ChessEngine.h"
#include "pthread.h"

typedef enum {
    ONE_V_ONE,
    ONE_V_AI
} GameMode;

typedef struct {

    int whiteTimeRemaining;
    int blackTimeRemaining;
    GameMode gameMode;
    Player turn;

} Game_Status;

extern  bool gameOver;
extern Player winner;
extern volatile bool helpPressed;

void GameController_init();
void GameController_cleanup();

GameMode GameController_getGameMode();
Player GameController_getTurn();
Player GameController_getWinner();
void GameController_getGameStatus(Game_Status *status);
char GameController_getCurrTurnString();
void waitUntilAIPhysicalMove(char* from, char* to);
void toggleTurn();
bool askStockfishForhelp(char *localFrom, char *localTo);

#endif