# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/chess_board.c" "src/test.c" )
file(GLOB MANAGER_SOURCES "src/sensor_game_engine_manager.c" "src/game_engine_manager.c" "src/chessHelper.c")

add_executable(extender ${MY_SOURCES})
add_executable(manager ${MANAGER_SOURCES})


# Make use of the HAL library
target_link_libraries(extender LINK_PRIVATE hal)
target_link_libraries(manager LINK_PRIVATE hal)


# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET extender POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:extender>"
     "~/cmpt433/public/myApps/extender" 
  COMMENT "Copying ARM executable to public NFS directory")

add_custom_command(TARGET manager POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:manager>"
     "~/cmpt433/public/myApps/manager" 
   COMMAND "${CMAKE_COMMAND}" -E copy 
     "${CMAKE_SOURCE_DIR}/hal/src/stockfish_game_engine"
     "$ENV{HOME}/cmpt433/public/myApps/stockfish_game_engine" 
  COMMENT "Copying manager and game engine executable to public NFS directory")

