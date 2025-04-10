#include "led_controller.h"
#include "stdio.h"
#include "stdbool.h"
#include "sharedDataLayoutLinux.h"
#include <assert.h>

#define NEO_NUM_LEDS 128


static bool isInit = false;

static LED_Color colorInfo[6] = {
    { .colorName = LED_COLOR_NONE,  .colorHex = 0x00000000 },
    { .colorName = LED_COLOR_WHITE, .colorHex = 0xffffff00 },
    { .colorName = LED_COLOR_RED,   .colorHex = 0x00ff0000 },
    { .colorName = LED_COLOR_GREEN, .colorHex = 0xff000000 },
    { .colorName = LED_COLOR_BLUE,  .colorHex = 0x0000ff00 },
    { .colorName = LED_COLOR_YELLOW,.colorHex = 0x0f0f000f },
};

static uint32_t colors[NEO_NUM_LEDS] = {0x0};

void led_init(){
    isInit = true;
    sharedMem_init();
   
}

void led_cleanup(){
    assert(isInit);
    isInit = false;
    sharedMem_cleanup();
}

// changes the color and triggers R5 to light up LED
void led_changeLedColor(int *colorArr){
    assert(isInit);

    printf("inside led change color\n");
    for(int i = 0; i< NEO_NUM_LEDS; i++){
        colors[i] = colorInfo[colorArr[i]].colorHex;
        // printf("colors[%d] = %d\n", i, colorArr[i]);
    }    
    sharedMem_changeLed(colors);
}