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
#include "game_engine_manager.h"
#include "logic_led_manager.h"
#include "joystick_help.h"

#define MAX_HELP_PER_PLAYER 5
#define MAX_TIME_IN_SEC 600 // 10 minutes
#define NEO_NUM_LEDS 64

// static int whiteHelpCount = 0;
// static int blackHelpCount = 0;

static GameMode gameMode = ONE_V_ONE;
static pthread_mutex_t gameModeMutex = PTHREAD_MUTEX_INITIALIZER;

static Player turn = PLAYER_WHITE;

static int whiteTimeRemaining = MAX_TIME_IN_SEC;
static int blackTimeRemaining = MAX_TIME_IN_SEC;

Player winner = 2;
bool gameOver = false;
static bool isGameModeSet = false;
// ADDED:
static char from[3];
static char to[3];
static bool isUserInCheck = false;
static bool isUserInCheckmate = false;
static bool isEngineInCheck = false;
static bool isEngineInCheckmate = false;


static pthread_t gameThread;

static uint8_t prevState[8][8] = {0};

static int totalMoves = 0;

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

void toggleTurn(){
    pthread_mutex_lock(&timerLock);
    if(turn == PLAYER_WHITE){
        whitePlayerTimer.running = false;
        turn = PLAYER_BLACK;
        blackPlayerTimer.running = true;
    }else{
        blackPlayerTimer.running = false;
        turn = PLAYER_WHITE;
        whitePlayerTimer.running = true;
    }
    pthread_mutex_unlock(&timerLock);

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

bool askStockfishForhelp(char *localFrom, char *localTo){
    if(isGameModeSet) {
        if(gameMode == ONE_V_ONE || (gameMode == ONE_V_AI && turn == PLAYER_WHITE)){
            bool localIsUserInCheck = false;
            bool localIsUserInCheckmate = false;

            Piece boardState[8][8];
            ChessEngine_getState(boardState); 
            Game_engine_manager_processBoardState(boardState, totalMoves, localFrom, localTo, &localIsUserInCheck, &localIsUserInCheckmate);
            return true;
        }
    }
    else {
       memset(localFrom, 0, 3 * sizeof(char));
       memset(localTo, 0, 3 * sizeof(char));
       return false;
    }
    return false;
}

// ADDED:
static void checkIfCurrUserIsInCheckOrCheckmate(){
        // ADDED:
        // want to check after every turn (AI's or human's (in case of V1V)), if there was a check/checkmate
        Piece boardState[8][8];
        ChessEngine_getState(boardState); 
        Game_engine_manager_processBoardState(boardState, totalMoves, from, to, &isUserInCheck, &isUserInCheckmate);

        if(isUserInCheck){
            // show move by lighting up lights
            printf("%d is in check\n",turn);
            
            // light up led of the king of the player who is in check

            // functionality to show the best move they can make
            // the move is already in from and to chars
            printf("Stockfish recommends from inside check method %s to %s", from, to);

            LIGHT_UP leds[2] = {0};
            LogicLedManager_makeStructForMove(leds, from , to);
            LogicLedManager_changeColor(leds, 2);


        }

        // ADDED:
        if(isUserInCheckmate){
            gameOver = true;
            winner = !turn;
            printf("Winnner is %d\n", winner);
            LogicLedManager_turnAllLeds(LED_COLOR_GREEN);
        }
        printf("Check/Checkmate check completed inside check mathod.\n");
}

static void waitUntilPieceReturnedToOriginalSquare() {
    
    uint8_t currState [8][8];
    
    while (1) {
        boardReader_getState(currState);
        bool pieceReturned = true;
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                if (prevState[rank][file] != currState[rank][file]) {
                    pieceReturned = false;
                    break;
                }
            }
            if (!pieceReturned) break;
        }

        if (pieceReturned) break;

        usleep(10000);
    }
}

static int squareToRow(char rank) {
    return rank - '1';
}

static int squareToCol(char file) {
    return file - 'a';
}

void waitUntilAIPhysicalMove(char* from, char* to) {
    int fromRow = squareToRow(from[1]);
    int fromCol = squareToCol(from[0]);
    int toRow   = squareToRow(to[1]);
    int toCol   = squareToCol(to[0]);

    // print fromRow, fromCol, toRow, toCol
    printf("FromRow: %d, FromCol: %d, ToRow: %d, ToCol: %d\n", fromRow, fromCol, toRow, toCol);

    printf("[AI-Move] Waiting for user to physically move piece from %s to %s...\n", from, to);

    int rank, file;
    MoveResult result;

    // STEP 1: Wait for correct pickup
    while (true) {
        if (boardReader_detectPickup(&rank, &file)) {
            // print tank and file
            printf("rank: %d, file: %d\n", rank, file);
            if (rank == fromRow && file == fromCol) {
                result = ChessEngine_ProcessMove(rank, file, turn);
                if (result == MOVE_PICKUP_VALID) {
                    printf("[AI-Move] Correct piece picked up.\n");
                    break;
                }
            } else {
                printf("[AI-Move] Incorrect pickup at %d,%d. Waiting for correct square.\n", rank, file);  
            }
        }
        usleep(10000);
    }

    // STEP 2: Wait for correct drop
    while (true) {
        if (boardReader_detectDrop(&rank, &file)) {
            printf("rank: %d, file: %d\n", rank, file);
            if (rank == toRow && file == toCol) {
                result = ChessEngine_ProcessMove(rank, file, turn);
                printf("result: %d\n", result);
                if (result == MOVE_DROP_VALID || result == MOVE_CAPTURE || result == MOVE_PROMOTION) {
                    printf("[AI-Move] Correct drop completed.\n");

                    if(result == MOVE_CAPTURE) {

                        printf("Piece captured!\n");
                        LogicLedManager_turnAllLeds(LED_COLOR_BLUE);
                        usleep(1000000);
                        LogicLedManager_turnAllLeds(LED_COLOR_NONE);

                    } else if (result == MOVE_PROMOTION) {

                        printf("Pawn promoted!\n");
                        LogicLedManager_turnAllLeds(LED_COLOR_YELLOW);
                        usleep(1000000);
                        LogicLedManager_turnAllLeds(LED_COLOR_NONE);
                    }
                    break;
                }
            } else {
                printf("[AI-Move] Incorrect drop at %d,%d. Waiting for %d,%d.\n", rank, file, toRow, toCol);
                printf("Pick up the piece again.\n");
                LIGHT_UP led = {0};
                int count;
                LogicLedManager_makeStructForOneLed(&led, rank, file, &count, LED_COLOR_RED);
                LogicLedManager_changeColor(&led, count);
                while(!boardReader_detectPickup(&rank, &file)) {
                    usleep(10000);
                }
                LogicLedManager_makeStructForOneLed(&led, rank, file, &count, LED_COLOR_NONE);
                LogicLedManager_changeColor(&led, count);
            }
        }
        usleep(10000);
    }

    printf("Done With AI Physical Move\n");
}

void* GameController_startGame() {
    // ChessEngine_init();

    while (!gameOver) {
        printf("\nTurn: %s\n", (turn == PLAYER_WHITE ? "WHITE" : "BLACK"));
        long turnStartTime = GameController_getCurrentTime();

        // ADDED:
        // maybe we dont need the conditional because this is the only loop that actually 
        // detects changes in the sensors
        // in case of ONE V AI and it being AIs turn. AI has decided the move but the player has 
        // to make the physical move - but this conditional is not activated then 
    
        if (gameMode == ONE_V_ONE || (gameMode == ONE_V_AI && turn == PLAYER_WHITE)) {
            int rank, file;
            MoveResult result;

            boardReader_getState(prevState);

            while (1) {
                if (!ChessEngine_isPieceInAir()) {
                    // Wait for pickup
                    if (boardReader_detectPickup(&rank, &file)) {
                        result = ChessEngine_ProcessMove(rank, file, turn);

                        if (result == MOVE_PICKUP_VALID) {
                            // Now wait for drop in next loop
                            // ADDED:
                            // show possible moves via the led
                            int possibleState[8][8];
                            ChessEngine_getPossibleMoves(rank, file, possibleState);

                            LIGHT_UP leds[NEO_NUM_LEDS] = {0};
                            int count; 
                            LogicLedManager_makeStructForPossibleMoves(leds, &count, possibleState);
                            LogicLedManager_changeColor(leds, count);

                        } else if (result == MOVE_PICKUP_INVALID) {
                            printf("Invalid pickup. Please return the piece to its original square.\n");
                            // ADDED:
                            // led should light up for invalid move - assuming rank is row and file is col of the 
                            // selected piece
                            LIGHT_UP led = {0};
                            int count;
                            LogicLedManager_makeStructForOneLed(&led, rank, file, &count, LED_COLOR_RED);
                            LogicLedManager_changeColor(&led, count);

                            waitUntilPieceReturnedToOriginalSquare();
                            LogicLedManager_turnAllLeds(LED_COLOR_NONE);
                            printf("Piece returned. Try again.\n");
                            // ADDED: 
                            // switch off the red led once the piece is returned


                        }
                    }
                } else {
                    // Wait for drop
                    if (boardReader_detectDrop(&rank, &file)) {
                        result = ChessEngine_ProcessMove(rank, file, turn);
                        // ADDED: Turn off led 
                        LogicLedManager_turnAllLeds(LED_COLOR_NONE);

                        if (result == MOVE_DROP_VALID) {
                            break;
                        } else if (result == MOVE_CAPTURE) {
                            printf("Piece captured!\n");
                            LogicLedManager_turnAllLeds(LED_COLOR_BLUE);
                            usleep(1000000);
                            LogicLedManager_turnAllLeds(LED_COLOR_NONE);
                            break;
                        } else if (result == MOVE_PROMOTION) {
                            printf("Pawn promoted!\n");
                            LogicLedManager_turnAllLeds(LED_COLOR_YELLOW);
                            usleep(1000000);
                            LogicLedManager_turnAllLeds(LED_COLOR_NONE);
                            break;
                        } else if (result == MOVE_GAME_OVER) {
                            gameOver = true;
                            // ADDED:
                            // leds turn on to show game over
                            break;
                        } else if (result == MOVE_DROP_INVALID) {
                            printf("Invalid drop. Please return the piece to its original square.\n");
                            // ADDED:
                            // leds turn on to show invalid
                            LogicLedManager_turnAllLeds(LED_COLOR_RED);

                            waitUntilPieceReturnedToOriginalSquare();
                            LogicLedManager_turnAllLeds(LED_COLOR_NONE);
                            printf("Piece returned. Try again.\n");

                            // ADDED:
                            // switch off led
                            LogicLedManager_turnAllLeds(LED_COLOR_NONE);
                        } else if (result == MOVE_CANCELLED) {
                            printf("Move cancelled. Re-pick a piece.\n");
                        }
                    }
                }

                usleep(10000);
            }

        } else {
            // --- AI Turn ---
            // ADDED:
            printf("AI thinking...\n");
            Piece boardState[8][8];
            ChessEngine_getState(boardState); // get the currstate to send to stockfish  
            // also checks if the engine is in check/checkmate by the user          
            Game_engine_manager_processBoardState(boardState, totalMoves, from, to, &isEngineInCheck, &isEngineInCheckmate);

            if(isEngineInCheckmate){
                // ADDED: 
                // user has won show green led all over and end game
                LogicLedManager_turnAllLeds(LED_COLOR_GREEN);
                gameOver = true;

            }

            if(isEngineInCheck){
                // ADDED:
                // show some light -- maybe black king had red light
                isEngineInCheck = false;
            }
             

            // if engine is in checkmate there are no moves
            if(!isEngineInCheckmate) {
                // not a check/checkmate, process stockfish move
                LIGHT_UP leds[2] = {0};
                LogicLedManager_makeStructForMove(leds, from, to);
                LogicLedManager_changeColor(leds,2);

                printf("AI recommends move from %s to %s\n", from, to);

                waitUntilAIPhysicalMove(from, to);
                LogicLedManager_turnAllLeds(LED_COLOR_NONE);

                printf("AI move completed.\n");
            }

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

        // ADDED: dont think this will work because you never actually capture the king
        // its a check mate when the next move is a capture 
        if (ChessEngine_isGameOver()) {
            printf("Game Over! %s's King captured.\n", (turn == PLAYER_WHITE ? "WHITE" : "BLACK"));
            gameOver = true;
            break;
        }

        toggleTurn(); // ✅ Only toggled after full pickup-drop turn

        // ADDED:
        totalMoves +=1;

        if(gameMode == ONE_V_ONE || (gameMode == ONE_V_AI && turn == PLAYER_WHITE) ){
            checkIfCurrUserIsInCheckOrCheckmate();
            if(isUserInCheck){
                printf("%d is in check\n",turn);
                printf("Stockfish recommends from outside is user in check %s to %s", from, to);

                waitUntilAIPhysicalMove(from, to);

                printf("Check move completed.\n");
                toggleTurn();
                isUserInCheck = false;

                printf("Turn toggled after check move.\n");
                printf("Turn: %s\n", (turn == PLAYER_WHITE ? "WHITE" : "BLACK"));
            }
            else {
                printf("User is not in check outside\n");
            }
        }

       
    }

    // ADDED:
    // commented it to be in sync with stockfish code
    // winner = (turn == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
    printf("Game Over! Winner: %s\n", winner == PLAYER_WHITE ? "WHITE" : "BLACK");

    lcd_printer_toogle_screen(RESULT_SCREEN);
    return NULL;
}

char GameController_getCurrTurnString(){
    return (turn == PLAYER_WHITE) ? 'w' : 'b';
}


void GameController_init(){
    printf("GameController_init\n");
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