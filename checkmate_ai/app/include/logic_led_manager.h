#ifndef _LOGIC_LED_MANAGER_H_
#define _LOGIC_LED_MANAGER_H_

#include <pthread.h>
#include <stdbool.h>
#include "led_controller.h"


// extern pthread_cond_t ledCondVar;
// extern pthread_mutex_t ledMutex;
// extern bool isChangeLed;

typedef struct {
    int row;
    int col; 
    LED_COLOR_NAME colorName;
} LIGHT_UP;


void LogicLedManager_init();
void LogicLedManager_cleanup();
void LogicLedManager_changeColor(LIGHT_UP *leds, int count);
void LogicLedManager_turnAllLeds(LED_COLOR_NAME ledColor);
void LogicLedManager_makeStructForPossibleMoves(LIGHT_UP *led, int *count, int board[8][8]);
void LogicLedManager_makeStructForMove(LIGHT_UP *leds, char *from, char *to);
void LogicLedManager_makeStructForOneLed(LIGHT_UP *leds, int row, int col, int *count, LED_COLOR_NAME colorName);


#endif