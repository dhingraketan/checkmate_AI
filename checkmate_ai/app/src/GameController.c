#define _POSIX_C_SOURCE 199309L

#include "GameController.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>  // for usleep()
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "joystick_controller.h"
#include "LCDPrinter.h"
#include "gpio.h"
#include "btn_statemachine.h"

#define MAX_HELP_PER_PLAYER 5
#define MAX_TIME_IN_SEC 600 // 10 minutes#include <unistd.h>



// static int whiteHelpCount = 0;
// static int blackHelpCount = 0;

//atomic variable for GameMode
static GameMode gameMode = ONE_V_ONE;
static pthread_mutex_t gameModeMutex = PTHREAD_MUTEX_INITIALIZER;

static Player turn = PLAYER_WHITE;

//10 minute timer for each player
static int whiteTimeRemaining = MAX_TIME_IN_SEC;
static int blackTimeRemaining = MAX_TIME_IN_SEC;

static Player winner = 2;
// static bool gameOver = false;
static bool isGameModeSet = false;

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



// static long GameController_getCurrentTime(){
//     struct timespec ts;
//     clock_gettime(CLOCK_MONOTONIC, &ts);
//     return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    
// }

// static void toggleTurn(){

//     if(turn == PLAYER_WHITE){
//         turn = PLAYER_BLACK;
//     }else{
//         turn = PLAYER_WHITE;
//     }
// }

void* joystickListener() {
    uint16_t prevY = joystick_get_y();

    while (!isGameModeSet) {
        uint16_t y_value = joystick_get_y();

        if ((y_value > 800 && prevY <= 800) || (y_value < 200 && prevY >= 200)) {
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

    // Create a thread to listen for joystick input
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


void* GameController_startGame(){

    return NULL;    
}



void GameController_init(){
    lcd_printer_toogle_screen(MODE_SELECTION_SCREEN);
    Gpio_initialize();
    BtnStateMachine_init();
    GameController_setGameMode();
    lcd_printer_toogle_screen(GAME_SCREEN);

}