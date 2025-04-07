#ifndef _SENSOR_GAME_ENGINE_MANAGER_H_
#define _SENSOR_GAME_ENGINE_MANAGER_H_

#include <pthread.h>
#include <stdbool.h>

typedef enum  {
    GAME_MODE_BEST_MOVE,
    GAME_MODE_PLAY_WITH_ENGINE,
    GAME_MODE_NUM
} GAME_MODE;


extern bool isCheck;
extern bool isCheckMate;


void * SensorGameEngineManager_init();
void SensorGameEngineManager_cleanup();


#endif