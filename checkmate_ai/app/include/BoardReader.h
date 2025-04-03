#ifndef _BOARD_READER_H_
#define _BOARD_READER_H_

#include <stdint.h>
#include <stdbool.h>

void boardReader_init();

void boardReader_cleanup();

void boardReader_startPolling();

void boardReader_getState(uint8_t state[8][8]);

bool boardReader_detectPickup(int* rank, int* file);

bool boardReader_detectDrop(int* rank, int* file);

bool boardReader_detectChange(int* rank, int* file);

#endif