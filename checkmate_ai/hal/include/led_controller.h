#ifndef _LED_CONTROLLER_H_
#define _LED_CONTROLLER_H_

#include <stdio.h>
#include <stdint.h>

typedef enum {
    LED_COLOR_NONE = 0,
    LED_COLOR_WHITE,
    LED_COLOR_RED,
    LED_COLOR_GREEN,
    LED_COLOR_BLUE,
    LED_COLOR_YELLOW,
    LED_COLOR_NUM,
} LED_COLOR_NAME;

typedef struct {
    LED_COLOR_NAME colorName;
    uint32_t colorHex;
} LED_Color;


void led_init();
void led_cleanup();
void led_changeLedColor(int *colorArr);

#endif