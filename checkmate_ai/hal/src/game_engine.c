#include "../include/game_engine.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define DEFAULT_DEPTH 2
#define STOCKFISH_EXECUTABLE_PATH "./stockfish_game_engine"

static bool isInit = false;
static int toEngine[2];
static int fromEngine[2];
static pid_t pid;
static char readBuffer[BUFFER_SIZE];
static char writeBuffer[BUFFER_SIZE];
static char cmdText[BUFFER_SIZE/2];
static const char* initCommands[] = {
    "uci\n",
    "ucinewgame\n",
    "position starpos\n",
    "setoption name UCI_ShowWDL value false\n",
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
        execlp(STOCKFISH_EXECUTABLE_PATH, "stockfish", NULL);

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

static void gameEngine_writeToPipe(char * writeText){
    memset(writeBuffer, 0, BUFFER_SIZE);
    memset(readBuffer, 0, BUFFER_SIZE);

    write(toEngine[1], writeText, strlen(writeText));
}

static void gameEngine_goCmd(){
    
    snprintf(cmdText, BUFFER_SIZE, "go depth %d\n", DEFAULT_DEPTH);
    gameEngine_writeToPipe(cmdText);

    ssize_t bytes_read;
    // printf("reading\n");
    while ((bytes_read = read(fromEngine[0], readBuffer, BUFFER_SIZE - 1)) > 0) {
        readBuffer[bytes_read] = '\0';
        // printf("curr %s", readBuffer);

        if (strstr(readBuffer, "bestmove")) {
            printf("this is the best move found %s\n", readBuffer);
            break;
        }

        memset(readBuffer, 0, BUFFER_SIZE);

    }
}

void gameEngine_sendCmd(GAME_ENGINE_CMDS cmd){
    if(cmd == CMD_GO){
        printf("sending go cmd\n");
        gameEngine_goCmd();
    }
}

void gameEngine_cleanup(){
    isInit = false;

    write(toEngine[1], "quit\n", 5);

    close(toEngine[1]);
    close(fromEngine[0]);

    int status;
    waitpid(pid, &status, 0);
}

// int main(){
    
//     printf("in init\n");
//     gameEngine_init();
//     printf("done init\n");
//     gameEngine_sendCmd(CMD_GO);
//     gameEngine_cleanup();

//     return 0;
// }