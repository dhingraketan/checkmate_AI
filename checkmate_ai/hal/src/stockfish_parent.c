#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


#define BUFFER_SIZE 2048
#define DEFAULT_DEPTH 2
int main(){
    // make pipes
    int toEngine[2];
    int fromEngine[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

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
        
        // Send commands to Stockfish
        const char* init_commands[] = {
            "uci\n",
            "ucinewgame\n",
            // "position startpos\n",
            // "go depth 10\n",
            NULL
        };

        for (int i = 0; init_commands[i] != NULL; i++) {
            write(toEngine[1], init_commands[i], strlen(init_commands[i]));
            usleep(100000); 
        }
        
        const char* position = "position startpos\n";
        char go[BUFFER_SIZE];
        snprintf(go, BUFFER_SIZE, "go depth %d\n", DEFAULT_DEPTH);
        
        write(toEngine[1], position, strlen(position));
        write(toEngine[1], go, strlen(go));
        
        // Read Stockfish's response
        ssize_t bytes_read;
        while ((bytes_read = read(fromEngine[0], buffer, BUFFER_SIZE - 1)) > 0) {
            buffer[bytes_read] = '\0';

            if (strstr(buffer, "bestmove")) {
                // Parse the best move here
                printf("this is the best move found %s\n", buffer);
                break;
            }
        }
    }
}