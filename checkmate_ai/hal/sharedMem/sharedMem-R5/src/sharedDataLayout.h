#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>

// R5 Shared Memory Note
// - It seems that using a struct for the ATCM memory does not work 
//   (hangs when accessing memory via a struct pointer).
// - Therefore, using an array.


#define MSG_OFFSET 0
#define MSG_SIZE   32
#define COUNT_OFFSET (MSG_OFFSET + MSG_SIZE)
#define DELAY_OFFSET (COUNT_OFFSET + sizeof(uint32_t))
#define BOOL_OFFSET (DELAY_OFFSET + sizeof(uint32_t))
#define LOOP_COUNT_OFFSET     (BOOL_OFFSET + sizeof(uint8_t) + 3)  
#define ARR_OFFSET (LOOP_COUNT_OFFSET + sizeof(uint32_t))
#define ARR_SIZE   (64 * sizeof(int32_t))
#define END_MEMORY_OFFSET (ARR_OFFSET + ARR_SIZE)

#define MEM_UINT8(addr) *(uint8_t*)(addr)
#define MEM_UINT32(addr) *(uint32_t*)(addr)
#endif
