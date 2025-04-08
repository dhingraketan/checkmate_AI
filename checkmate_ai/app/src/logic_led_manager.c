#include "logic_led_manager.h"
#include "led_controller.h"
#include "pthread.h"
#include "assert.h"
#include <string.h>

#define NEO_NUM_LEDS 128
static bool isInit = false;
static int colorArr[128];

static int LogicManager_findLedNumber(int row, int col){
    int ledsDoneInPrevRow = (row * 8);
    int ledsDoneInCurrRow = (row % 2 == 0) ? (7 - col) : col;
    int ledNumber = (ledsDoneInCurrRow + ledsDoneInPrevRow) * 2;
    printf("this is the led position i will light up %d\n", ledNumber);
    return ledNumber;
}

static void LogicLedManager_writeColorArr(LIGHT_UP *leds, int count){
    assert(isInit);

    int colors[NEO_NUM_LEDS] = {LED_COLOR_NONE};

    if(leds != NULL){
        for(int i = 0; i<count; i++){
            int ledNumber = LogicManager_findLedNumber(leds[i].row, leds[i].col);
            // int ledIndx = (((8 * leds[i].row) + leds[i].col) *2 ) - 2;
            colors[ledNumber] = leds[i].colorName;
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

void LogicLedManager_turnAllLeds(LED_COLOR_NAME ledColor){
    assert(isInit);
    printf("calling change all color logic led manager\n");

    int colors[NEO_NUM_LEDS] = {LED_COLOR_NONE};

    for(int i = 0; i < NEO_NUM_LEDS; i++){
        if(i % 2 == 0){ // every even led gets color
            colors[i] = ledColor;
        }
    }

    memcpy(colorArr, colors, sizeof(colorArr));
    printf("logic led manager turn all color %d\n",ledColor);
    led_changeLedColor(colorArr);
}

void LogicLedManager_makeStructForMove(LIGHT_UP *leds, char *from, char *to){
    printf("inside make struct for move %s %s\n", from , to);
    int fromRow = from[1] - '0' - 1;
    int fromCol = from[0] - 'a';

    int toRow = to[1] - '0' - 1;
    int toCol = to[0] - 'a';

    printf("from: %d %d. to: %d %d\n", fromRow, fromCol, toRow, toCol);


    LIGHT_UP ledFrom, ledTo;

    ledFrom.col = fromCol;
    ledFrom.row = fromRow;
    ledFrom.colorName = LED_COLOR_WHITE;

    ledTo.colorName = LED_COLOR_WHITE;
    ledTo.col = toCol;
    ledTo.row = toRow;

    leds[0] = ledFrom;
    leds[1] = ledTo;
}

void LogicLedManager_makeStructForPossibleMoves(LIGHT_UP *led, int *count, int board[8][8]){
    int indx = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            // printf("Possible[%d][%d] = %d\n", i, j , possible[i][j]);
            if(board[i][j]){
                LIGHT_UP possibleMoveLed = {i, j , LED_COLOR_WHITE};
                printf("light up %d %d = %d", i, j , LED_COLOR_WHITE);
                led[indx] = possibleMoveLed;
                indx++;
            }
        }
    }
    *count = indx;
}

void LogicLedManager_makeStructForOneLed(LIGHT_UP *leds, int row, int col, int *count, LED_COLOR_NAME colorName){
    leds->row = row;
    leds->col = col;
    leds->colorName = colorName;
    *count = 1;
}

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