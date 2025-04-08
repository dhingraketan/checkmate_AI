#include "logic_led_manager.h"
#include "led_controller.h"
#include "chessHelper.h"
#include "pthread.h"
#include "assert.h"

#define NEO_NUM_LEDS 128
static bool isInit = false;
static int colorArr[128];

static void LogicLedManager_writeColorArr(LIGHT_UP *leds, int count){
    assert(isInit);

    int colors[NEO_NUM_LEDS] = {COLOR_NONE};

    if(leds != NULL){
        for(int i = 0; i<count; i++){
            int ledIndx = (((8 * leds[i].row) + leds[i].col) *2 ) - 2;
            colors[ledIndx] = leds[i].colorName;
        }
    }
    memcpy(colorArr, colors, sizeof(colorArr));
}


void LogicLedManager_changeColor(LIGHT_UP *leds, int count){
    assert(isInit);
    printf("logic led manager change color %d\n", count);

    LogicLedManager_writeColorArr(leds, count);
    led_changeLedColor(colorArr);
}

void LogicLedManager_turnAllColor(LED_COLOR_NAME ledColor){
    assert(isInit);
    printf("calling change all color logic led manager\n");

    int colors[NEO_NUM_LEDS] = {COLOR_NONE};

    for(int i = 0; i < NEO_NUM_LEDS; i++){
        if(i % 2 == 1){
            colors[i] = ledColor;
        }
    }

    memcpy(colorArr, colors, sizeof(colorArr));
    printf("logic led manager turn all color color %d\n");
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
    printf("led manager cleanup\n");
    // pthread_join(threadId, NULL);
}