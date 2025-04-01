#include <sensor_game_engine_manager.h>
#include "chessHelper.h"
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

#define DEFAULT_GAME_MODE GAME_MODE_BEST_MOVE

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

int main(){
    printf("here2\n");
    pthread_create(&threadId, NULL, SensorGameEngineManager_init, NULL);

    SensorGameEngineManager_cleanup();
}