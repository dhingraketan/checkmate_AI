#include "logic_led_manager.h"
#include "led.h"
#include "chessHelper.h"
#include "pthread.h"

static bool isInit = false;
static int possible[8][8];
static int colorArr[64];
static pthread_t threadId;

static void LogicLedManager_writeColorArr(){
    int indx = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(possible[i][j]){
                colorArr[indx++] = COLOR_WHITE;
            }
            else {
                colorArr[indx++] = COLOR_NONE;
            }
        }
    }
}

static void LogicLedManager_changeColor(){
    LogicLedManager_writeColorArr();
    led_changeLedColor(colorArr);
}

void * LogicLedManager_makeThread(){
    printf("made led thread\n");
    while(1){
        printf("trying to get the lock\n");
        pthread_mutex_lock(&ledMutex);

        while (!isChangeLed) {
            printf("waiting on cond var led\n");
            pthread_cond_wait(&ledCondVar, &ledMutex);
            printf("[LED THREAD] woke up from cond var\n");
        }
        printf("woken up in led manager thread\n");
        // a piece is in air - get possible moves/board state
        copyPossibleMoves(possible);
        LogicLedManager_changeColor();

        isChangeLed = false;
        pthread_mutex_unlock(&ledMutex);

    }
    printf("ending\n");
    return NULL;
}


void LogicLedManager_init(){
    isInit = true;
    printf("init led manager\n");
    led_init();
    pthread_create(&threadId,NULL, LogicLedManager_makeThread, NULL);
}



void LogicLedManager_cleanup(){
    isInit = false;
    printf("cleanup led\n");

    pthread_join(threadId, NULL);
}