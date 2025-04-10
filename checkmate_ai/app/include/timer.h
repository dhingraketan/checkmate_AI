#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "pthread.h"
// #include "GameController.h"

typedef struct {
    int minutes;
    int seconds;
    bool running;
    time_t last_update;
    // Player player;
} PlayerTimer;

extern PlayerTimer whitePlayerTimer;
extern PlayerTimer blackPlayerTimer;
extern pthread_mutex_t timerLock;


void Timer_init();
void Timer_cleanup();


#endif