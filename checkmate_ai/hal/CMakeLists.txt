# CMakeList.txt for HAL
#   Build a library (`hal`) which exposes the header files as "hal/*.h"
#   Use header as: #include "hal/button.h"

include_directories(hal/include)
file(GLOB MY_SOURCES "src/*.c"
${CMAKE_CURRENT_SOURCE_DIR}/sharedMem/sharedMem-Linux/sharedMem-Linux.c)


add_library(hal STATIC ${MY_SOURCES})

target_include_directories(hal PUBLIC include  ${CMAKE_CURRENT_SOURCE_DIR}/sharedMem/sharedMem-Linux/include)
