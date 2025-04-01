#ifndef _PORT_EXTENDER_H_
#define _PORT_EXTENDER_H_

#include <stdint.h>

void portExtender_init_and_config();
void portExtender_readAllPins(uint8_t state[8][8]);
void portExtender_cleanup();

#endif // PORT_EXTENDER_H