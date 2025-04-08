#include "timer.h"
#include "stdio.h"
#include <unistd.h>

static int turn;

static void * toggleTurn(){
    while(1){
        // pthread_mutex_lock(&timerLock);
        if(turn == 1){
            whitePlayerTimer.running = false;
            turn = 0;
            blackPlayerTimer.running = true;
        }else{
            blackPlayerTimer.running = false;
            turn = 1;
            whitePlayerTimer.running = true;
        }
        usleep(2000000); 

        // pthread_mutex_unlock(&timerLock);
}
return NULL;

}

int main(){
    turn = 0;
    pthread_t thread;

    Timer_init();
    pthread_create(&thread,NULL,toggleTurn, NULL);
    while(1){
    

        
        if(turn){
            printf("White time: %02d:%02d\n", whitePlayerTimer.minutes, whitePlayerTimer.seconds);
        }
        else {
            printf("black time: %02d:%02d\n", whitePlayerTimer.minutes, whitePlayerTimer.seconds);

        }
        usleep(1000);

    }

}