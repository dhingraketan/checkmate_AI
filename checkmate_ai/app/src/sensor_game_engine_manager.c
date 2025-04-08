#include "sensor_game_engine_manager.h"
#include "chessHelper.h"
#include "BoardReader.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <assert.h>

// #define DEFAULT_GAME_MODE GAME_MODE_BEST_MOVE

// static bool isInit = false;
// static int currGameMode = DEFAULT_GAME_MODE;

// static pthread_t gameThread;
// pthread_t threadId;
// // static pthread_t gameEngineThread;


// void * SensorGameEngineManager_init(){
//     isInit = true;
//     // init game logic
//     printf("here in sensor game engine manager create\n");
//     LogicLedManager_init();
//     printf("here in sensor game engine manager create 1\n");

//     Game_engine_manager_init();
//     printf("here in sensor game engine manager create 2\n");

//     pthread_create(&gameThread, NULL, chessGameThread, NULL); 
//     printf("here in sensor game engine manager create 3\n");


//     while (1){};
    
//     return NULL;

// }

// void SensorGameEngineManager_cleanup(){
//     // assert(isInit);
//     // pthread_join(gameThread, NULL);

//     printf("here3\n");
//     pthread_join(threadId, NULL);
//     pthread_join(gameThread, NULL);
//     Game_engine_manager_cleanup();
//     LogicLedManager_cleanup();
//     printf("cleanup done\n");
    
//     isInit = false;
// }

// // int main(){
// //     printf("here2\n");
// //     boardReader_init();
// //     printf("here4\n");

// //     int result = pthread_create(&threadId, NULL, SensorGameEngineManager_init, NULL);

// //     // SensorGameEngineManager_cleanup();
// //     if (result != 0) {
// //         // Error occurred
// //         fprintf(stderr, "pthread_create failed: %d\n", result);
// //         return 1;
// //     }
// //     SensorGameEngineManager_cleanup();
// //     printf("done\n");
// // }
