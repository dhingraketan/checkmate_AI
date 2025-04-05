#include "../include/game_engine.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define DEFAULT_DEPTH 10
#define STOCKFISH_EXECUTABLE_PATH "./stockfish_game_engine"

static bool isInit = false;
static int toEngine[2];
static int fromEngine[2];
static pid_t pid;
static char readBuffer[BUFFER_SIZE];
static char writeBuffer[BUFFER_SIZE];
static char cmdText[BUFFER_SIZE];
static char lookForText[BUFFER_SIZE];

static const char* initCommands[] = {
    "uci\n",
    "isready\n",
    "ucinewgame\n",
    "position starpos\n",
    "setoption name UCI_ShowWDL value false\n",
    NULL
};

static bool gameEngine_alive(pid_t pid) {
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);

    // printf("checking if its alive\n");

    if (result == 0) return true;  

    if (result == -1) {
        perror("waitpid");
        return false;
    }
    return false;  
}

void gameEngine_init(){
    isInit = true;
    printf("low level game engine init\n");

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

static void gameEngine_talkToGameEngine(char * cmdText, char *lookFor, char *lineFound){
    memset(writeBuffer, 0, BUFFER_SIZE);
    memset(readBuffer, 0, BUFFER_SIZE);

    write(toEngine[1], cmdText, strlen(cmdText));
    // printf("here1\n");

    ssize_t bytes_read;
    // char accumulatedOutput[BUFFER_SIZE * 4] = {0};

    // printf("here4\n");

    // printf("reading\n");
    if(lookFor != NULL && lookFor[0] != '\0'){
        printf("going back\n");
        while ((bytes_read = read(fromEngine[0], readBuffer, BUFFER_SIZE - 1)) > 0) {
            readBuffer[bytes_read] = '\0';
            // printf("curr %s", readBuffer);


            if (strstr(readBuffer, lookFor)) {
                char *line = strtok(readBuffer, "\n");
                while (line != NULL) {
                    if (strstr(line, lookFor)) {
                        printf("Found in line: %s\n", line);
                        strncpy(lineFound, line, strlen(line));
                        break;
                    }
                    line = strtok(NULL, "\n");
                }
                break;
            }
            // printf("here3\n");
            memset(readBuffer, 0, BUFFER_SIZE);

        }
    }
    printf("done writing and reading \n");
}


void gameEngine_sendCmd(GAME_ENGINE_CMDS cmd, char *fenString, char *returnLine){
    memset(cmdText, 0, BUFFER_SIZE);
    memset(lookForText, 0, BUFFER_SIZE);

    if(!gameEngine_alive(pid)) return;

    if(cmd == CMD_GO){
        // printf("sending go cmd 1\n");
        snprintf(cmdText, BUFFER_SIZE, "go depth %d\n", DEFAULT_DEPTH );
        snprintf(lookForText, BUFFER_SIZE, "bestmove");

    }
    else if(cmd == CMD_POSITION){
        // printf("sending position cmd\n");
        snprintf(cmdText, BUFFER_SIZE, "position fen %s\n", fenString );
        // snprintf(cmdText, BUFFER_SIZE, "isready\n");
        // snprintf(lookForText, BUFFER_SIZE, "readyok\n");
    }
    else if(cmd == CMD_IS_READY){
        // printf("sending is ready cmd\n");
        snprintf(cmdText, BUFFER_SIZE, "isready\n");
        snprintf(cmdText, BUFFER_SIZE, "isready\n");
        snprintf(lookForText, BUFFER_SIZE, "readyok\n");
    }
    else if(cmd == CMD_D){
        snprintf(cmdText, BUFFER_SIZE, "d");
    }
    // printf("talking\n");

    gameEngine_talkToGameEngine(cmdText, lookForText, returnLine);
    // printf("talking done\n");

}

void gameEngine_cleanup(){
    isInit = false;
    printf("in cleanup\n");
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