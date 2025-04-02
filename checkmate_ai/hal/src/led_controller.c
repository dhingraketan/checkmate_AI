#include "led_controller.h"
#include "stdio.h"
#include "stdbool.h"
#include "sharedDataLayoutLinux.h"
#include <assert.h>

#define NEO_NUM_LEDS 64


static bool isInit = false;

static LED_Color colorInfo[6] = {
    { .colorName = COLOR_WHITE, .colorHex = 0x0f0f0f00 },
    { .colorName = COLOR_RED,   .colorHex = 0x000f0000 },
    { .colorName = COLOR_GREEN, .colorHex = 0x0f000000 },
    { .colorName = COLOR_BLUE,  .colorHex = 0x00000f00 },
    { .colorName = COLOR_YELLOW,.colorHex = 0x0f0f0000 },
    { .colorName = COLOR_NONE,  .colorHex = 0x00000000 },

};

static uint32_t colors[NEO_NUM_LEDS] = {0x0};

void led_init(){
    isInit = true;
    sharedMem_init();
   
}

void led_cleanup(){
    assert(isInit);
    isInit = false;
}

// changes the color and triggers R5 to light up LED
void led_changeLedColor(int *colorArr){
    assert(isInit);

    printf("inside led change color\n");
    for(int i = 0; i< NEO_NUM_LEDS; i++){
        colors[i] = colorInfo[colorArr[i]].colorHex;
    }    
    // for(int i = 0; i< NEO_NUM_LEDS; i++){
    //     printf("changed colorArr[%d]: 0x%08x\n", i, colors[i]);
    // }  
    sharedMem_changeLed(colors);
}