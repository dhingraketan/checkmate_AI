#ifndef _SENSOR_GAME_ENGINE_MANAGER_H_
#define _SENSOR_GAME_ENGINE_MANAGER_H_

#include <pthread.h>
#include <stdbool.h>

typedef enum  {
    GAME_MODE_BEST_MOVE,
    GAME_MODE_PLAY_WITH_ENGINE,
    GAME_MODE_NUM
} GAME_MODE;


extern pthread_cond_t stockfishTurnCond;
extern pthread_cond_t userTurnCond;
extern bool isStockfishTurn;
extern bool isUserTurn;
extern pthread_mutex_t boardMutex;


void * SensorGameEngineManager_init();
void SensorGameEngineManager_cleanup();


#endif