#ifndef _JOYSTICK_CONTROLLER_H_
#define _JOYSTICK_CONTROLLER_H_

#include <stdint.h>

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NO_MOVEMENT
} J_DIRECTION;

void joystick_init();
uint16_t joystick_get_x();
uint16_t joystick_get_y();
J_DIRECTION joystick_get_direction();
void joystick_cleanup();

#endif