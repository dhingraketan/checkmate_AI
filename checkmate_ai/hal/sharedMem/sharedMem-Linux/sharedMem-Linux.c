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

void sharedMem_init(){
    printf("init sharedmem\n");
    isInit = true;
    pR5Base = getR5MmapAddr();
}

void sharedMem_cleanup(){
    assert(isInit);
    freeR5MmapAddr();
}
void sharedMem_changeLed(int32_t *colorArr){
    assert(isInit);
    printf("changing shared mem\n");
    for(int i = 0; i<64; i++){
        MEM_UINT32((uint8_t*)pR5Base + ARR_OFFSET + i* sizeof(uint32_t)) = colorArr[i];
    }
    MEM_UINT8((uint8_t*)pR5Base+ BOOL_OFFSET) = 1;
    printf("got loop count from r5 %d", MEM_UINT32((uint8_t*)pR5Base+ LOOP_COUNT_OFFSET));

}

int main1(void)
{
    printf("Sharing memory with R5\n");
    printf("  LED should change speed every 5s.\n");
    printf("  Press the button to see its state here.\n");

    // Get access to shared memory for my uses
    volatile void *pR5Base = getR5MmapAddr();

	// printf("Contents of memory :\n");
	// for (int i = 0; i < 50; i++) {
	// 	volatile char* addr = pR5Base + i;
    //     char val = *addr;
    //     printf("Offset %d = %3d (char '%c')\n", i, val, val);
	// }

    // // Print out the mem contents:
    // printf("From the R5, memory hold:\n");
    // // NOTE: Cannot access it as a string, gives "Bus error"
    // //printf("    %15s: \"%s\"\n", "msg", (char*)(pR5Base + MSG_OFFSET));
    // printf("    %15s: 0x%04x\n", "delay", MEM_UINT32(pR5Base + DELAY_OFFSET));
    // printf("    %15s: 0x%04x\n", "bool ", MEM_UINT8(pR5Base + BOOL_OFFSET));


    MEM_UINT32((uint8_t*)pR5Base + DELAY_OFFSET) = 100;
    // Drive it
    for (int i = 0; true; i++) {
        // Set LED timing
        printf("here\n");
        // if(MEM_UINT8(pR5Base + BOOL_OFFSET) == 0){
        //     printf("bust wait\n");
        // }
        // MEM_UINT8(pR5Base + BOOL_OFFSET) = 0;
        // printf("set it to 0\n");
        // MEM_UINT32(pR5Base + DELAY_OFFSET) = (i % 10 < 5) ? 100 : 250;

        // // Print button
        // printf("Loop Count: %7d\n", 
        //     MEM_UINT32(pR5Base + LOOP_COUNT_OFFSET)
        // );

        // Timing
        sleep(1);
    }

    // Cleanup
    freeR5MmapAddr();
}