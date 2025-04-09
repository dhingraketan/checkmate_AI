#include "joystick_help.h"
#include "joystick_controller.h"
#include "GameController.h"
#include "stdbool.h"
#include "pthread.h"
#include "logic_led_manager.h"

static bool isInit = false;
static pthread_t threadId;
static char from[3];
static char to[3];

static void*joystick_help_startThread(){
    // start monitoring the joystick
    while(!gameOver){
        J_DIRECTION currDir = joystick_get_direction();
        if(currDir == LEFT){ 
            // ask stockfish and display move
            printf("joystick left action triggered\n");
            if(askStockfishForhelp(from, to)) {
                LIGHT_UP leds[2] = {0};
                LogicLedManager_makeStructForMove(leds, from, to);
                LogicLedManager_changeColor(leds,2);
            }
            else {
                printf("mode not set not doing enyhting\n");
            }
           
        }
    }
    return NULL;
}

void joystick_help_init(){
    isInit = true;
    printf("joystick help init\n");
    pthread_create(&threadId, NULL, joystick_help_startThread, NULL);
}

void joystick_help_cleanup(){
    pthread_join(threadId, NULL);
    isInit = false;
}
