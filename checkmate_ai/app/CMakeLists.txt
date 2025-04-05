# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
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