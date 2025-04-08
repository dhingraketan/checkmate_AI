#include "timer.h"

#include "pthread.h"
#include "time.h"
#include <unistd.h>

bool isInit = false;
static pthread_t timerThread; 
pthread_mutex_t timerLock = PTHREAD_MUTEX_INITIALIZER;
PlayerTimer whitePlayerTimer = {5, 0, true, 0};  // Player 1 starts
PlayerTimer blackPlayerTimer = {5, 0, false, 0};




// current->running = false;
// next->running = true;
// next->last_update = time(NULL);

static void Timer_updateTimer(PlayerTimer* timer) {
    if (!timer->running) return;

    time_t now = time(NULL);
    int elapsed = now - timer->last_update;
    if (!timer->running || elapsed <= 0) return;

    if (elapsed > 0) {
        timer->last_update = now;

        int total_seconds = timer->minutes * 60 + timer->seconds - elapsed;
        if (total_seconds < 0) total_seconds = 0;

        timer->minutes = total_seconds / 60;
        timer->seconds = total_seconds % 60;

        // Optional: stop timer if it hits zero
        if (total_seconds == 0) {
            timer->running = false;
            // gameOver = true;
            // winner = !timer->player;
            
            printf("Time's up!\n");
        }
    }
}


void* Timer_startThread() {
   
    // while (!gameOver) {
    while (1) {

        pthread_mutex_lock(&timerLock);
        Timer_updateTimer(&whitePlayerTimer);
        Timer_updateTimer(&blackPlayerTimer);
        pthread_mutex_unlock(&timerLock);

        usleep(200000); // 0.5 second interval
    }
    return NULL;
}

void Timer_init(){
    isInit = true;


    whitePlayerTimer.minutes = 10;
    whitePlayerTimer.seconds = 0;
    whitePlayerTimer.running = true;
    whitePlayerTimer.last_update = time(NULL);
    // whitePlayerTimer.player = PLAYER_WHITE;

    blackPlayerTimer.minutes = 10;
    blackPlayerTimer.seconds = 0;
    blackPlayerTimer.running = false;
    blackPlayerTimer.last_update = time(NULL);
    // blackPlayerTimer.player = PLAYER_BLACK;

    pthread_mutex_init(&timerLock, NULL);



    pthread_create(&timerThread, NULL, Timer_startThread, NULL);

}
void Timer_cleanup(){

    pthread_mutex_destroy(&timerLock);
    pthread_join(timerThread, NULL);

}