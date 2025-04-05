#ifndef _GAME_CONTROLLER_H_
#define _GAME_CONTROLLER_H_

typedef enum {
    ONE_V_ONE,
    ONE_V_AI
} GameMode;

typedef enum {
    PLAYER_WHITE,
    PLAYER_BLACK

} Player;

typedef struct {

    int whiteTimeRemaining;
    int blackTimeRemaining;
    GameMode gameMode;
    Player turn;

} Game_Status;

void GameController_init();

GameMode GameController_getGameMode();
Player GameController_getTurn();
Player GameController_getWinner();
void GameController_getGameStatus(Game_Status *status);

#endif