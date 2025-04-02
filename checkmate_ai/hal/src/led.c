#include "led.h"
#include "stdio.h"
#include "stdbool.h"

#define NUM_LEDS 64


static bool isInit = false;

static LED_Color colorInfo[6] = {
    { .colorName = COLOR_WHITE, .colorHex = 0x0f0f0f00 },
    { .colorName = COLOR_RED,   .colorHex = 0x000f0000 },
    { .colorName = COLOR_GREEN, .colorHex = 0x0f000000 },
    { .colorName = COLOR_BLUE,  .colorHex = 0x00000f00 },
    { .colorName = COLOR_YELLOW,.colorHex = 0x0f0f0000 },
    { .colorName = COLOR_NONE,  .colorHex = 0x00000000 },

};

static int32_t colors[NUM_LEDS] = {0x0};

void led_init(){
    isInit = true;
   
}

void led_cleanup(){
    isInit = false;
}

// changes the color and triggers R5 to light up LED
void led_changeLedColor(int *colorArr){
    printf("isnide led change color\n");
    for(int i = 0; i< NUM_LEDS; i++){
        colors[i] = colorInfo[colorArr[i]].colorHex;
    }    
}