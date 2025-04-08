// #define _POSIX_C_SOURCE 199309L

#include "GameController.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> 
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "joystick_controller.h"
#include "BoardReader.h"
#include "LCDPrinter.h"
#include "gpio.h"
#include "btn_statemachine.h"

#define MAX_HELP_PER_PLAYER 5
#define MAX_TIME_IN_SEC 600 // 10 minutes

// static int whiteHelpCount = 0;
// static int blackHelpCount = 0;

static GameMode gameMode = ONE_V_ONE;
static pthread_mutex_t gameModeMutex = PTHREAD_MUTEX_INITIALIZER;

static Player turn = PLAYER_WHITE;

static int whiteTimeRemaining = MAX_TIME_IN_SEC;
static int blackTimeRemaining = MAX_TIME_IN_SEC;

static Player winner = 2;
static bool gameOver = false;
static bool isGameModeSet = false;

static pthread_t gameThread;

GameMode GameController_getGameMode(){
    pthread_mutex_lock(&gameModeMutex);
    GameMode mode = gameMode;
    pthread_mutex_unlock(&gameModeMutex);
    return mode;
}

Player GameController_getTurn(){
    return turn;
}

Player GameController_getWinner(){
    return winner;
}

void GameController_getGameStatus(Game_Status *status){
    status->whiteTimeRemaining = whiteTimeRemaining;
    status->blackTimeRemaining = blackTimeRemaining;
    status->gameMode = gameMode;
    status->turn = turn;
}



static long GameController_getCurrentTime(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    
}

static void toggleTurn(){

    if(turn == PLAYER_WHITE){
        turn = PLAYER_BLACK;
    }else{
        turn = PLAYER_WHITE;
    }
}

void* joystickListener() {
    uint16_t prevY = joystick_get_y();

    while (!isGameModeSet) {
        uint16_t y_value = joystick_get_y();

        if ((y_value > 1200 && prevY <= 1200) || (y_value < 200 && prevY >= 200)) {
            pthread_mutex_lock(&gameModeMutex);
            gameMode = (gameMode == ONE_V_ONE) ? ONE_V_AI : ONE_V_ONE;
            pthread_mutex_unlock(&gameModeMutex);
        }

        prevY = y_value;

    }

    return NULL;
}

static void GameController_setGameMode(){
    joystick_init();

    pthread_t joystickThread;
    pthread_create(&joystickThread, NULL, joystickListener, NULL);

    int counter = 0;
    while(!isGameModeSet){
        BtnStateMachine_doState();
        counter++;
        if(counter % 2 == 0){
            isGameModeSet = true;
            counter = 0;
        }
    }



    pthread_join(joystickThread, NULL);
    joystick_cleanup();

}

static bool pieceReturnedToOriginalSquare() {
    int row, col;
    ChessEngine_getPickupSquare(&row, &col);

    if (row < 0 || row >= 8 || col < 0 || col >= 8) {
        // Invalid square, nothing to check
        return false;
    }

    uint8_t sensorState[8][8];
    boardReader_getState(sensorState);

    return sensorState[row][col] == 1;
}

static bool waitUntilPieceReturnedToOriginalSquare(int timeout_ms) {
    const int interval_us = 50000;
    int waited_ms = 0;

    while (!pieceReturnedToOriginalSquare()) {
        usleep(interval_us);
        waited_ms += interval_us / 1000;

        if (waited_ms >= timeout_ms) {
            printf("Timeout while waiting for piece return.\n");
            return false;
        }
    }

    return true;
}

void* GameController_startGame() {
    ChessEngine_init();

    while (!gameOver) {
        printf("\nTurn: %s\n", (turn == PLAYER_WHITE ? "WHITE" : "BLACK"));
        long turnStartTime = GameController_getCurrentTime();

        if (gameMode == ONE_V_ONE || (gameMode == ONE_V_AI && turn == PLAYER_WHITE)) {
            int rank, file;
            MoveResult result;

            // Wait until a full move (pickup + drop) is done
            while (1) {
                if (!ChessEngine_isPieceInAir()) {
                    // Wait for pickup
                    if (boardReader_detectPickup(&rank, &file)) {
                        result = ChessEngine_ProcessMove(rank, file, turn);

                        if (result == MOVE_PICKUP_VALID) {
                            // Now wait for drop in next loop
                        } else if (result == MOVE_PICKUP_INVALID) {
                            printf("Invalid pickup. Please return the piece to its original square.\n");
                            if (!waitUntilPieceReturnedToOriginalSquare(5000)) {
                                // Optional: recover from stuck state
                                printf("Proceeding after timeout.\n");
                                // Could add a ChessEngine_resetInAir() here if you want to clear internal engine state
                            }
                            printf("Piece returned. Try again.\n");
                        }
                    }
                } else {
                    // Wait for drop
                    if (boardReader_detectDrop(&rank, &file)) {
                        result = ChessEngine_ProcessMove(rank, file, turn);

                        if (result == MOVE_DROP_VALID) {
                            break;
                        } else if (result == MOVE_CAPTURE) {
                            printf("Piece captured!\n");
                            break;
                        } else if (result == MOVE_PROMOTION) {
                            printf("Pawn promoted!\n");
                            break;
                        } else if (result == MOVE_GAME_OVER) {
                            gameOver = true;
                            break;
                        } else if (result == MOVE_DROP_INVALID) {
                            printf("Invalid drop. Please return the piece to its original square.\n");
                            if (!waitUntilPieceReturnedToOriginalSquare(5000)) {
                                // Optional: recover from stuck state
                                printf("Proceeding after timeout.\n");
                                // Could add a ChessEngine_resetInAir() here if you want to clear internal engine state
                            }
                            printf("Piece returned. Try again.\n");
                        } else if (result == MOVE_CANCELLED) {
                            printf("Move cancelled. Re-pick a piece.\n");
                        }
                    }
                }

                usleep(10000);
            }

        } else {
            // --- AI Turn ---
            printf("AI thinking...\n");
            sleep(1);
            printf("AI move completed.\n");

            // TODO: Replace with actual Stockfish move
        }

        // Time tracking logic
        long turnEndTime = GameController_getCurrentTime();
        int elapsedSeconds = (turnEndTime - turnStartTime) / 1000;

        if (turn == PLAYER_WHITE) {
            whiteTimeRemaining -= elapsedSeconds;
            if (whiteTimeRemaining <= 0) {
                printf("White timed out.\n");
                winner = PLAYER_BLACK;
                gameOver = true;
                break;
            }
        } else {
            blackTimeRemaining -= elapsedSeconds;
            if (blackTimeRemaining <= 0) {
                printf("Black timed out.\n");
                winner = PLAYER_WHITE;
                gameOver = true;
                break;
            }
        }

        if (ChessEngine_isGameOver()) {
            printf("Game Over! %s's King captured.\n", (turn == PLAYER_WHITE ? "WHITE" : "BLACK"));
            gameOver = true;
            break;
        }

        toggleTurn(); // ✅ Only toggled after full pickup-drop turn
    }

    winner = (turn == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
    printf("Game Over! Winner: %s\n", winner == PLAYER_WHITE ? "WHITE" : "BLACK");

    lcd_printer_toogle_screen(RESULT_SCREEN);
    return NULL;
}


void GameController_init(){
    lcd_printer_toogle_screen(MODE_SELECTION_SCREEN);
    Gpio_initialize();
    BtnStateMachine_init();
    GameController_setGameMode();
    lcd_printer_toogle_screen(GAME_SCREEN);
    ChessEngine_init();

    printf("Chess Game Initialized\n");
    pthread_create(&gameThread, NULL, GameController_startGame, NULL);
    printf("Game thread started\n");

}

void GameController_cleanup(){

    pthread_join(gameThread, NULL);
    ChessEngine_cleanup();
    lcd_printer_stop_and_cleanup();
    BtnStateMachine_cleanup();
    Gpio_cleanup();
}