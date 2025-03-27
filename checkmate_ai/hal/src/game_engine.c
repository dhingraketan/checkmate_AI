#include "game_engine.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define DEFAULT_DEPTH 2

static bool isInit = false;
static int toEngine[2];
static int fromEngine[2];
static pid_t pid;
static char buffer[BUFFER_SIZE];
static const char* initCommands[] = {
    "uci\n",
    "ucinewgame\n",
    NULL
};

void gameEngine_init(){
    isInit = true;

    if(pipe(toEngine) == -1 || pipe(fromEngine)){
        perror("Pipe not created\n");
        exit(EXIT_FAILURE);
    }

    // make child process
    pid = fork();
    if(pid == -1){
        perror("Cant create child process\n");
        exit(EXIT_FAILURE);
    }
    // if child process, make pipe input/output and start stockfish
    if(pid == 0){
        close(toEngine[1]); //close the write side of this pipe because we are never writing
        close(fromEngine[0]); //close the read end of this pipe
        
        dup2(toEngine[0], STDIN_FILENO); 
        dup2(fromEngine[1], STDOUT_FILENO);
        
        // Execute Stockfish
        execlp("./stockfish", "stockfish", NULL);

        perror("execlp");
        exit(EXIT_FAILURE);

    }
    // if parent process, communicate with stockfish and wait for response
    else {
        close(toEngine[0]);
        close(fromEngine[1]);
        
        // send commands
        for (int i = 0; initCommands[i] != NULL; i++) {
            write(toEngine[1], initCommands[i], strlen(initCommands[i]));
            usleep(100000); 
        }
    }

}

void gameEngine_cleanup(){
    isInit = false;
}