#include <sensor_game_engine_manager.h>
#include "chessHelper.h"
#include "BoardReader.h"
#include <game_engine_manager.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <ctype.h>
#include "draw_stuff.h"

#define DEFAULT_GAME_MODE GAME_MODE_BEST_MOVE

static pthread_t lcd;

static bool isInit = false;
static int currGameMode = DEFAULT_GAME_MODE;

static pthread_t gameThread;
pthread_t threadId;
// static pthread_t gameEngineThread;


void * SensorGameEngineManager_init(){
    isInit = true;
    // init game logic
    printf("here\n");
    pthread_create(&gameThread, NULL, chessGameThread, NULL); 
    Game_engine_manager_init(currGameMode);
    while (1){};
    
    return NULL;

}

void SensorGameEngineManager_cleanup(){
    // pthread_join(gameThread, NULL);

    printf("here3\n");
    pthread_join(threadId, NULL);
    
    isInit = false;
}

#define START_TIME 600  // 10 minutes in seconds

bool running = true;
int whiteTimeRemaining = START_TIME;
int blackTimeRemaining = START_TIME;

void* lcd_update_thread(void* arg) {
    (void)arg;
    char lcdBuff[264];
    time_t lastUpdateTime = time(NULL);

    while (running) {
        time_t currentTime = time(NULL);
        int delta = (int)(currentTime - lastUpdateTime);
        lastUpdateTime = currentTime;

        // Only update the timer for the player whose turn it is.
        if (delta > 0) {
            if (strcmp(currentPlayer, "White") == 0) {
                whiteTimeRemaining -= delta;
                if (whiteTimeRemaining < 0)
                    whiteTimeRemaining = 0;
            } else if (strcmp(currentPlayer, "Black") == 0) {
                blackTimeRemaining -= delta;
                if (blackTimeRemaining < 0)
                    blackTimeRemaining = 0;
            }
        }

        int whiteMin = whiteTimeRemaining / 60;
        int whiteSec = whiteTimeRemaining % 60;
        int blackMin = blackTimeRemaining / 60;
        int blackSec = blackTimeRemaining % 60;
        snprintf(lcdBuff, sizeof(lcdBuff),
            " Timer for White: %02d:%02d\n"
            "                \n"
            "                \n"
            "                \n"
            " Timer for Black: %02d:%02d\n"
            "  Turn : %s",
            whiteMin, whiteSec,
            blackMin, blackSec,
            currentPlayer);
        DrawStuff_updateScreen(lcdBuff);
        sleep(1);  // Update every second
    }
    return NULL;
}

int main(){
    DrawStuff_init();
    running = true;
    pthread_create(&lcd, NULL, lcd_update_thread, NULL);
    printf("here2\n");
    boardReader_init();
    pthread_create(&threadId, NULL, SensorGameEngineManager_init, NULL);

    SensorGameEngineManager_cleanup();
    running = false;
    pthread_join(lcd, NULL);
    DrawStuff_cleanup();
}