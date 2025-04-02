/*
 * R5 Sample Code for Shared Memory with Linux
 */

#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <string.h>

#include "sharedDataLayout.h"

#define NEO_NUM_LEDS          64  // # LEDs in our string
#define DELAY_TIME_uS   (500 * 1000)


// NeoPixel Timing
// NEO_<one/zero>_<on/off>_NS
// (These times are what the hardware needs; the delays below are hand-tuned to give these).
#define NEO_ONE_ON_NS       700   // Stay on 700ns
#define NEO_ONE_OFF_NS      600   // (was 800)
#define NEO_ZERO_ON_NS      350
#define NEO_ZERO_OFF_NS     800   // (Was 600)
#define NEO_RESET_NS      60000   // Must be at least 50us, use 60us

// Delay time includes 1 GPIO set action.
volatile int junk_delay = 0;
#define DELAY_350_NS() {}
#define DELAY_600_NS() {for (junk_delay=0; junk_delay<9 ;junk_delay++);}
#define DELAY_700_NS() {for (junk_delay=0; junk_delay<16 ;junk_delay++);}
#define DELAY_800_NS() {for (junk_delay=0; junk_delay<23 ;junk_delay++);}

#define DELAY_NS(ns) do {int target = k_cycle_get_32() + k_ns_to_cyc_near32(ns); \
	while(k_cycle_get_32() < target) ; } while(false)

#define NEO_DELAY_ONE_ON()     DELAY_700_NS()
#define NEO_DELAY_ONE_OFF()    DELAY_600_NS()
#define NEO_DELAY_ZERO_ON()    DELAY_350_NS()
#define NEO_DELAY_ZERO_OFF()   DELAY_800_NS()
#define NEO_DELAY_RESET()      {DELAY_NS(NEO_RESET_NS);}


// Memory
// ----------------------------------------
#define SHARED_MEM_BTCM_START 0x00000000  // TRM p848
#define SHARED_MEM_ATCM_START 0x00041010  // TRM p849
static void *pSharedMem = (void *) SHARED_MEM_BTCM_START;

// Access GPIO (for demonstration purposes)
// ----------------------------------------
// 1,000,000 uSec = 1000 msec = 1 sec
#define MICRO_SECONDS_PER_MILI_SECOND   (1000)
#define DEFAULT_LED_DELAY_MS            (100)

// Device tree nodes for pin aliases
#define LED0_NODE DT_ALIAS(led0)
#define BTN0_NODE DT_ALIAS(btn0)
#define NEOPIXEL_NODE DT_ALIAS(neopixel)


static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);
static const struct gpio_dt_spec neopixel = GPIO_DT_SPEC_GET(NEOPIXEL_NODE, gpios);

static void initialize_gpio(const struct gpio_dt_spec *pPin, int direction) 
{
	if (!gpio_is_ready_dt(pPin)) {
		printf("ERROR: GPIO pin not ready read; direction %d\n", direction);
		exit(EXIT_FAILURE);
	}

	int ret = gpio_pin_configure_dt(pPin, direction);
	if (ret < 0) {
		printf("ERROR: GPIO Pin Configure issue; direction %d\n", direction);
		exit(EXIT_FAILURE);
	}
}

static void changeAll(uint32_t myColor){
	for (int i = 0; i < NEO_NUM_LEDS; i++) {
		// MEM_UINT32((((uint8_t*)pSharedMem )+ ARR_OFFSET) + (i * sizeof(uint32_t))) = 0x0f000000;
		uint32_t *addr = (((uint8_t*)pSharedMem )+ ARR_OFFSET) + (i * sizeof(uint32_t));
		MEM_UINT32(addr) = myColor;
		// MEM_UINT32(addr) = 0x00000000;
		// *addr = 0x0f000000;
		// *addr = 0x00000000;
	}
}

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);

	initialize_gpio(&led, GPIO_OUTPUT_ACTIVE);
	initialize_gpio(&btn, GPIO_INPUT);
	initialize_gpio(&neopixel, GPIO_OUTPUT_ACTIVE);

	

	// printf("Contents of Shared Memory ATCM:\n");
	// for (int i = 0; i < END_MEMORY_OFFSET; i++) {
	// 	char* addr = (char*)pSharedMem + i;
	// 	printf("0x%08x = %2x (%c)\n", (uint32_t) addr, *addr, *addr);
	// }

	// Addresses:
	// printf("Addresses\n");
	// printf("  %20s = 0x%08x\n", "msg", MSG_OFFSET);
	// printf("  %20s = 0x%08x\n", "ledDelay_ms", DELAY_OFFSET);
	// printf("  %20s = 0x%08x\n", "loopCount", LOOP_COUNT_OFFSET);

	// Setup defaults
	// printf("Writing to ATCM...\n");
	// printf("   ..1\n");
	// strcpy((char*) MSG_OFFSET, "Hello from R5 World (Take 6)!");

	// make it all 0s so it doesnt read garbage values
	for (int i = 0; i < NEO_NUM_LEDS; i++) {
		// MEM_UINT32((((uint8_t*)pSharedMem )+ ARR_OFFSET) + (i * sizeof(uint32_t))) = 0x0f000000;
		uint32_t *addr = (((uint8_t*)pSharedMem )+ ARR_OFFSET) + (i * sizeof(uint32_t));
		MEM_UINT32(addr) = 0x0f000000;
		// MEM_UINT32(addr) = 0x00000000;
		// *addr = 0x0f000000;
		// *addr = 0x00000000;
	}

	MEM_UINT32(DELAY_OFFSET) = 0;
	MEM_UINT32(LOOP_COUNT_OFFSET) = 0;
	MEM_UINT32(BOOL_OFFSET) = 0;
	
	// printf("Contents of Shared Memory ATCM After Write:\n");
	// for (int i = 0; i < END_MEMORY_OFFSET; i++) {
	// 	char* addr = (char*)pSharedMem + i;
	// 	printf("0x%08x = %2x (%c)\n", (uint32_t) addr, *addr, *addr);
	// }


	bool led_state = true;
	uint32_t outsideLoopCount = 0;
	uint32_t loopCount = 0;
	uint32_t color[NEO_NUM_LEDS] = {0};
	bool valuesGiven = false;
	uint32_t buttonCount = 0;

	uint32_t colorSet[8] = {
        0x0f000000, // Green
        0x000f0000, // Red
        0x00000f00, // Blue
        0x0000000f, // White
        0x0000000f, // White (via RGB)
        0x0f0f0000, // Yellow
        0x000f0f00, // Purple
        0x0f000f00, // Teal
    };  

	gpio_pin_set_dt(&neopixel, 0);
	NEO_DELAY_RESET();
	int tick = 0;
	bool flag = 0;

	while (true) {
		// // Toggle LED
		printf("LED state: %s\n", led_state ? "OFF" : "ON");
		if (gpio_pin_toggle_dt(&led) < 0) {
			printf("ERROR: GPIO Pin Toggle DT\n");
			return 0;
		}

		led_state = !led_state;
		int state = gpio_pin_get_dt(&btn);
		bool isPressed = state == 0;

		// Update shared count of butten pressed
		// if (isPressed) {
		// 	buttonCount++;
		// }
		// MEM_UINT32(DELAY_OFFSET) = buttonCount;


		// while(MEM_UINT32(BOOL_OFFSET) == 0){}

		
		// 	printf("busy wait in r5");
		// // uint32_t color[NEO_NUM_LEDS];
		outsideLoopCount++;
		MEM_UINT32(DELAY_OFFSET) = outsideLoopCount;
		// valuesGiven = false;
		uint32_t useShared = MEM_UINT32(BOOL_OFFSET);

		// if(useShared){
		// 	for(int i = 0; i<NEO_NUM_LEDS; i++){
		// 		// color[i] = MEM_UINT32(((uint8_t*)pSharedMem +ARR_OFFSET) + (i * sizeof(uint32_t)));
		// 		color[i] = 0x0f000000;
		// 	}
		// 	MEM_UINT32(BOOL_OFFSET) = 0;

		// }
		// else {
		// 	for(int i = 0; i<NEO_NUM_LEDS; i++){
		// 		color[i] = color[i] = colorSet[(i + tick) % 8];
		// 	}
			
		// }
		for(int i = 0; i< NEO_NUM_LEDS; i++){

			int32_t *addr = ((uint8_t*)pSharedMem +ARR_OFFSET) + (i* sizeof(uint32_t));
			// // int32_t *addr2 = addr + (i* sizeof(uint32_t));
			// // int32_t colorGreen = MEM_UINT32(addr2);
			int32_t colorGreen = MEM_UINT32(addr);
			color[i] = colorGreen;


			// int32_t *addr = (((uint8_t*)pSharedMem +ARR_OFFSET) + (i * sizeof(uint32_t)));
			// *addr = 0x00000f00;
			// int32_t colorGreen = MEM_UINT32(addr);
			// color[i] = colorGreen;
			// color[i] = 0x00000f00;

		}

		// color[0] = MEM_UINT32(((uint8_t*)pSharedMem + ARR_OFFSET) + (0 * sizeof(uint32_t)));

		// MEM_UINT32(BOOL_OFFSET) = 0;
		loopCount++;
		MEM_UINT32(LOOP_COUNT_OFFSET) = loopCount;

		// if(!valuesGiven){
		// 	continue;
		// }

		// strcpy((char*) MSG_OFFSET, "Hello");


		gpio_pin_set_dt(&neopixel, 0);
			
		DELAY_NS(NEO_RESET_NS);

		for(int j = 0; j < NEO_NUM_LEDS; j++) {
			for(int i = 31; i >= 8; i--) {
				if(color[j] & ((uint32_t)0x1 << i)) {
					gpio_pin_set_dt(&neopixel, 1);
					NEO_DELAY_ONE_ON();
					gpio_pin_set_dt(&neopixel, 0);
					NEO_DELAY_ONE_OFF();
				} else {
					gpio_pin_set_dt(&neopixel, 1);
					NEO_DELAY_ZERO_ON();
					gpio_pin_set_dt(&neopixel, 0);
					NEO_DELAY_ZERO_OFF();
				}
			}
		}

		gpio_pin_set_dt(&neopixel, 0);
		NEO_DELAY_RESET();

		// k_busy_wait(delay * MICRO_SECONDS_PER_MILI_SECOND);	
		
		
		// k_busy_wait(1 * MICRO_SECONDS_PER_MILI_SECOND);
		// uint32_t myColor = (flag == 0) ? 0x0f000000 : 0x000f0000;
		// changeAll(myColor);
		// flag = !flag;
	}
	return 0;
}
