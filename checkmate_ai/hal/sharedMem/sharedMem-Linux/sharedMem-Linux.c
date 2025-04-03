#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>

#include <string.h>

#include "sharedDataLayoutLinux.h"

// General R5 Memomry Sharing Routine
// ----------------------------------------------------------------
#define ATCM_ADDR     0x79000000  // MCU ATCM (p59 TRM)
#define BTCM_ADDR     0x79020000  // MCU BTCM (p59 TRM)
#define MEM_LENGTH    0x8000
#define NEO_NUM_LEDS      128

static volatile void *pR5Base;
static bool isInit = false;
// Return the address of the base address of the ATCM memory region for the R5-MCU
volatile void* getR5MmapAddr(void)
{
    // Access /dev/mem to gain access to physical memory (for memory-mapped devices/specialmemory)
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem; Did you run with sudo?");
        exit(EXIT_FAILURE);
    }

    // Inside main memory (fd), access the part at offset BTCM_ADDR:
    // (Get points to start of R5 memory after it's memory mapped)
    pR5Base = mmap(0, MEM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BTCM_ADDR);
    if (pR5Base == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pR5Base;
}

void freeR5MmapAddr()
{
    if (munmap((void*) pR5Base, MEM_LENGTH)) {
        perror("R5 munmap failed");
        exit(EXIT_FAILURE);
    }
}

static void sharedMem_readColorsFromMem(){
    for(int i = 0; i< 30;i++){
        printf("read loop count %u\n", MEM_UINT32(((uint8_t*)pR5Base + LOOP_COUNT_OFFSET)));
    }
    printf("reading what r5 has\n");
    for(int i = 0; i<4; i++){
        for(int i = 0; i< NEO_NUM_LEDS; i++){
            uint32_t color = MEM_UINT32(((int8_t *)pR5Base + ARR_OFFSET) + (i * sizeof(uint32_t)));
            printf("%d - 0x%08x\n", i, color);
        }
    }
    int count = 0;
    while(1){
        uint32_t myColor = (count %2 == 0) ? 0x000f0000 : 0x00000f00;
        // printf("Changing color to 0x%08x\n", myColor);
        MEM_UINT32(((int8_t *)pR5Base + ARR_OFFSET) + (3 * sizeof(uint32_t))) = myColor;
        count++;
        sleep(1);
    }
}

void sharedMem_init(){
    // printf("init sharedmem\n");
    isInit = true;
    pR5Base = getR5MmapAddr(); 
}

void sharedMem_cleanup(){
    assert(isInit);
    freeR5MmapAddr();
}
void sharedMem_changeLed(uint32_t *colorArr){

    for(int i = 0; i< NEO_NUM_LEDS; i++){
        
        uint32_t write_val = colorArr[i];

        MEM_UINT32(((int8_t *)pR5Base + ARR_OFFSET) + (i * sizeof(uint32_t))) = write_val;
    }

}
