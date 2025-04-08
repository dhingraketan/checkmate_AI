# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c" )
# file(GLOB TIME_SOURCES   "src/timer.c" "src/testTimer.c" )

list(REMOVE_ITEM MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/chessHelper.c")
list(REMOVE_ITEM MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/testTimer.c")


# add_executable(extender ${MY_SOURCES})
add_executable(extender ${MY_SOURCES})


# Make use of the HAL library
target_link_libraries(extender LINK_PRIVATE hal)
target_link_libraries(extender LINK_PRIVATE lgpio)
target_link_libraries(extender LINK_PRIVATE gpiod)
target_link_libraries(extender LINK_PRIVATE lcd)
# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET extender POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:extender>"
     "~/cmpt433/public/myApps/extender" 
  COMMENT "Copying ARM executable to public NFS directory")