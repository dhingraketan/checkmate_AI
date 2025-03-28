#ifndef _SENSOR_GAME_ENGINE_MANAGER_H_
#define _SENSOR_GAME_ENGINE_MANAGER_H_

typedef enum  {
    GAME_MODE_BEST_MOVE,
    GAME_MODE_PLAY_WITH_ENGINE,
    GAME_MODE_NUM
} GAME_MODE;

char *fenString[7] =  {"-", "p", "r", "n", "b", "q", "k"}; // this has to match the PieceType enum

static void * SensorGameEngineManager_init();
static void SensorGameEngineManager_cleanup();


#endif