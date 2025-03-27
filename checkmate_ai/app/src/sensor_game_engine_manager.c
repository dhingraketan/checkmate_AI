#include <sensor_game_engine_manager.h>
#include <chess_board.h>
#include "game_engine.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include "main.c"
#include <ctype.h>

#define MAX_FEN_STRING_LEN 128
static bool isInit = false;
static atomic_char boadStateFenString[MAX_FEN_STRING_LEN] = "";
static bool whiteCanCastleToKingSide = false;
static bool blackCanCastleToKingSide = false;
static bool whiteCanCastleToQueenSide = false;
static bool blackCanCastleToQueenSide = false;

static atomic_int totalMoves = 1;

static void * SensorGameEngineManager_init(){
    isInit = true;
    // init game logic
    gameEngine_init();
    while(1){
        char input[256];
        if (fgets(input, sizeof(input), stdin) != NULL) { 
            // change this to game logic
            printf("user input %s\n", input);
            // if it is a legal move then we get the board state from game logic
            // convert this board state to a fen string 
            if(currentTurn == BLACK){
                totalMoves +=1;
            }
            // send updated position to the game engine
            // get the best move
           
            // convert the move to a fen string to send it to the game engine
            // go
            gameEngine_sendCmd(CMD_GO);
        }
    }
    return NULL;

}

static void SensorGameEngineManager_getFenString(Piece board[8][8]){
    // # Loop over each row of the board from the 8th rank to the 1st
    int fenStringIndex = 0;
    for(int i = 0; i < 8; i++){
        int emptyCount = 0;

        for(int j = 0; j < 8; j++){
            // If the square is empty, increment the counter
            if (board[i, j]->color == NONE) {
                emptyCount +=1;
            }
            else {
                // # If the square is not empty and there were any empty squares before it,
                // # add the count of empty squares to the FEN string before adding the piece
                if (emptyCount > 0) {
                    boadStateFenString[fenStringIndex] = (char) emptyCount;
                    emptyCount = 0;
                    fenStringIndex +=1;
                }

                Piece currPiece = board[i][j];
                int pieceType = board[i][j].type;

                SensorGameEngineManager_kingMoveCheck(currPiece);
                
                if(currPiece.color == WHITE){
                    
                    boadStateFenString[fenStringIndex] = toupper(fenString[pieceType]);

                }
                boadStateFenString[fenStringIndex] = fenString[pieceType];
                fenStringIndex +=1;


            }
        }
        // # Add empty squares at the end of the row
        if (emptyCount > 0) {
            boadStateFenString[fenStringIndex] = (char) emptyCount;
            fenStringIndex +=1;
        }

        // Add a slash to separate rows, unless this is the last row
        if (i != 1) {
            boadStateFenString[fenStringIndex] = (char) '/';
            fenStringIndex +=1;
        }
    
    }
    if(currentTurn == WHITE){
        boadStateFenString[fenStringIndex] = (char) 'b';
        fenStringIndex +=1;
    }
    else{
        boadStateFenString[fenStringIndex] = (char) 'w';
        fenStringIndex +=1;

    }
    if(whiteCanCastleToKingSide){
        boadStateFenString[fenStringIndex] = (char) 'K';
        fenStringIndex +=1;
    }

    if(whiteCanCastleToQueenSide){
        boadStateFenString[fenStringIndex] = (char) 'Q';
        fenStringIndex +=1;
    }

    if(blackCanCastleToKingSide){
        boadStateFenString[fenStringIndex] = (char) 'k';
        fenStringIndex +=1;
    }

    if(blackCanCastleToQueenSide){
        boadStateFenString[fenStringIndex] = (char) 'q';
        fenStringIndex +=1;
    }
    // w KQkq - 0 1

}

// change this to check the side as well
static void SensorGameEngineManager_kingMoveCheck(Piece currPiece){
    if(currPiece.type == KING && currPiece.moved){
        if(currPiece.color == WHITE){
            whiteCanCastleToKingSide = false;
        }
        else{
            whiteCanCastleToQueenSide = false;
        }
    }

    if(currPiece.type == ROOK && currPiece.moved){
        if(currPiece.color == WHITE){
            whiteCanCastleToKingSide = false;
        }
        else{
            whiteCanCastleToQueenSide = false;
        }
    }
}


void SensorGameEngineManager_cleanup(){
    isInit = false;
}

int main(){
    pthread_t threadId;
    pthread_create(&threadId, NULL, SensorGameEngineManager_init, NULL);

    SensorGameEngineManager_init();
    SensorGameEngineManager_cleanup();
}