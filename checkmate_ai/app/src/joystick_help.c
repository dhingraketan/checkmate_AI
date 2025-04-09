#include "joystick_help.h"
#include "joystick_controller.h"
#include "GameController.h"
#include "stdbool.h"
#include "pthread.h"
#include "logic_led_manager.h"
#include "btn_statemachine.h"

static bool isInit = false;
static pthread_t threadId;
static char from[3];
static char to[3];

static void*joystick_help_startThread(){
    // start monitoring the joystick
    int counter = 0;
    while(1){
        printf("joystick help thread starting\n");
        BtnStateMachine_doState();
        printf("joystick help thread\n");

        counter++;
        if(counter % 2 == 0){
            // ask stockfish and display move
            printf("joystick left action triggered\n");
            if(askStockfishForhelp(from, to)) {
                LIGHT_UP leds[2] = {0};
                LogicLedManager_makeStructForMove(leds, from, to);
                LogicLedManager_changeColor(leds,2);
                printf("AI HELP");
                waitUntilAIPhysicalMove(from, to);
                toggleTurn();
                // LogicLedManager_turnAllLeds(LED_COLOR_NONE);
                printf("AI HELP DONE\n");
            }
            else {
                printf("mode not set not doing enyhting\n");
            }
            counter = 0;


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
    printf("joystick help cleanup\n");
    pthread_join(threadId, NULL);
    isInit = false;
}
