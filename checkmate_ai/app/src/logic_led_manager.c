#include "logic_led_manager.h"
#include "led_controller.h"
#include "chessHelper.h"
#include "pthread.h"
#include "assert.h"

#define NEO_NUM_LEDS 128
static bool isInit = false;
static int possible[8][8];
static int colorArr[128];
static pthread_t threadId;

static void LogicLedManager_writeColorArr(LIGHT_UP *leds){
    assert(isInit);

    bool isValid = chessHelper_getIsValidMove();

    int indx = 0;
    int colors[NEO_NUM_LEDS] = {0};

    if(leds != NULL){
        int count = sizeof(leds)/sizeof(leds[0]);

        for(int i = 0; i<count; i++){
            int ledIndx = (((8 * leds[i].row) + leds[i].col) *2 )- 2;
            colors[ledIndx] = leds[i].colorName;
        }
    }
    memcpy(colorArr, colors, sizeof(colorArr));
}



//     if(isValid){
//         for(int i = 0; i < 8; i++){
//             for(int j = 0; j < 8; j++){
//                 if(possible[i][j]){
//                     colorArr[indx++] = COLOR_WHITE;
//                 }
//                 else {
//                     colorArr[indx++] = COLOR_NONE;
//                 }
//                 colorArr[indx++] = COLOR_NONE;
//             }
//         }
//     }
//     else {
//         for(int i = 0; i < 8; i++){
//             for(int j = 0; j < 8; j++){
//                 colorArr[indx++] = COLOR_RED;
//                 colorArr[indx++] = COLOR_NONE;
//             }
//         }
//     }
// }

void LogicLedManager_changeColor(LIGHT_UP *leds){
    assert(isInit);

    LogicLedManager_writeColorArr(leds);
    led_changeLedColor(colorArr);
}

// void * LogicLedManager_makeThread(){
//     assert(isInit);
    
//     while(1){
//         pthread_mutex_lock(&ledMutex);

//         while (!isChangeLed) {
//             pthread_cond_wait(&ledCondVar, &ledMutex);
//         }

//         isChangeLed = false;

//         copyPossibleMoves(possible);
//         LogicLedManager_changeColor(LIGHT_UP *leds);
//         pthread_mutex_unlock(&ledMutex);

//     }
//     return NULL;
// }


void LogicLedManager_init(){
    isInit = true;
    led_init();
    // pthread_create(&threadId,NULL, LogicLedManager_makeThread, NULL);
}



void LogicLedManager_cleanup(){
    assert(isInit);

    isInit = false;
    led_cleanup();
    // pthread_join(threadId, NULL);
}