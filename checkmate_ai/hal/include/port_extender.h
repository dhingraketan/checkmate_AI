#ifndef _PORT_EXTENDER_H_
#define _PORT_EXTENDER_H_

#include <stdint.h>

void portExtender_init_and_config();
void portExtender_readAllPins(int portNumber, uint8_t portData[16]);
void portExtender_cleanup();

#endif // PORT_EXTENDER_H