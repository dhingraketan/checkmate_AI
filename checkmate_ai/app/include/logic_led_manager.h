#ifndef _LOGIC_LED_MANAGER_H_
#define _LOGIC_LED_MANAGER_H_

#include <pthread.h>
#include <stdbool.h>


extern pthread_cond_t ledCondVar;
extern pthread_mutex_t ledMutex;
extern bool isChangeLed;


void LogicLedManager_init();
void LogicLedManager_cleanup();
// void LogicLedManager_writeColorArr();


#endif