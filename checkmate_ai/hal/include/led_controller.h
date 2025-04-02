#ifndef _LED_CONTROLLER_H_
#define _LED_CONTROLLER_H_

#include <stdio.h>
#include <stdint.h>

typedef enum {
    COLOR_WHITE = 0,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_NONE,
    COLOR_NUM,
} LED_COLOR_NAME;

typedef struct {
    LED_COLOR_NAME colorName;
    uint32_t colorHex;
} LED_Color;


void led_init();
void led_cleanup();
void led_changeLedColor(int *colorArr);

#endif