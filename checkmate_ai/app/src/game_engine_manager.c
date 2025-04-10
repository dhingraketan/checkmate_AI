#include "game_engine_manager.h"
#include "game_engine.h"
#include "ChessEngine.h"
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

#define MAX_FEN_STRING_LEN 128

static bool isInit  = false;
static char boadStateFenString[MAX_FEN_STRING_LEN] ;
static int totalMoves  = 1;
static char *fenString[7] =  {"-", "p", "r", "n", "b", "q", "k"}; // this has to match the PieceType enum
static char returnLine[MAX_FEN_STRING_LEN] = {0};

// got this from https://rdrr.io/github/dryguy/rbitr/src/R/board_to_fen.R
static void GameEngineManager_getFenString(Piece board[8][8]) {
    int fenIndex = 0;
    memset(boadStateFenString, 0, MAX_FEN_STRING_LEN);

    // Piece placement
    for (int i = 7;i >= 0; i--) {
        int emptyCount = 0;
        
        for (int j = 0; j < 8; j++) {
            Piece currPiece = board[i][j];

            if (currPiece.type == EMPTY) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    boadStateFenString[fenIndex++] = '0' + emptyCount;
                    emptyCount = 0;
                }
                char pieceChar = *fenString[currPiece.type];
                boadStateFenString[fenIndex++] = 
                    (currPiece.color == COLOR_WHITE) ? toupper(pieceChar) : pieceChar;
            }
        }
        
        if (emptyCount > 0) {
            boadStateFenString[fenIndex++] = '0' + emptyCount;
        }
        
        if (i > 0) {
            boadStateFenString[fenIndex++] = '/';
        }
    }

    // Active color
    boadStateFenString[fenIndex++] = ' ';
    boadStateFenString[fenIndex++] = GameController_getCurrTurnString();

    // Castling availability
    boadStateFenString[fenIndex++] = ' ';

    boadStateFenString[fenIndex++] = '-';

    // En passant
    boadStateFenString[fenIndex++] = ' ';
    boadStateFenString[fenIndex++] = '-'; 

    // Halfmove clock
    boadStateFenString[fenIndex++] = ' ';
    boadStateFenString[fenIndex++] = '0';

    // Fullmove number
    boadStateFenString[fenIndex++] = ' '; // change this to have actual move numbers
    snprintf(boadStateFenString + fenIndex, MAX_FEN_STRING_LEN - fenIndex, "%d", totalMoves);
    
    boadStateFenString[MAX_FEN_STRING_LEN-1] = '\0';
}

static void Game_engine_manager_parseOutput(char *line, char *from, char *to, bool *isCheck, bool *isCheckMate){
    char bestmove[6];
    printf("this is line %s\n", line);
    if (strstr(line, "none") != NULL) {
        *isCheckMate = true;
        printf("game is over. checkmate\n");
        return;
    }

    char *after = strstr(line, "Checkers:");

    if (after) {
        after += strlen("Checkers:"); 
        while (*after == ' ') after++; 

        if (*after != '\0') {
            printf("There is something after Checkers: %s\n", after);
            *isCheck = true;
        } 
    }

    sscanf(line, "bestmove %4s", bestmove);

    strncpy(from, bestmove, 2); // Copy "e2"
    from[2] = '\0';

    strncpy(to, bestmove + 2, 2); // Copy "e4"
    to[2] = '\0';
}

void Game_engine_manager_processBoardState(Piece boardState[8][8], int totalMoves, char *from, char *to, bool *isCheck, bool *isCheckMate){
    totalMoves = totalMoves;
    GameEngineManager_getFenString(boardState);
    printf("this is the fen string %s\n", boadStateFenString);

    memset(returnLine, 0, MAX_FEN_STRING_LEN);

    // send curr pos to stockfish
    gameEngine_sendCmd(CMD_POSITION, boadStateFenString,returnLine);
    memset(returnLine, 0, MAX_FEN_STRING_LEN);

    // check for check by the user to the game engine
    gameEngine_sendCmd(CMD_D, boadStateFenString,returnLine); 
    Game_engine_manager_parseOutput(returnLine, from, to, isCheck, isCheckMate);
    memset(returnLine, 0, MAX_FEN_STRING_LEN);

    // get the best move
    printf("sending go cmd\n");
    gameEngine_sendCmd(CMD_GO, NULL,returnLine);

    printf("Bestmove found %s\n ", returnLine);
    Game_engine_manager_parseOutput(returnLine, from, to, isCheck, isCheckMate);

    printf("from: %s, to: %s\n", from, to);

}

void Game_engine_manager_init(){
    isInit = true;
    gameEngine_init();
    printf("init game engine manager\n");

}


void Game_engine_manager_cleanup(){
    printf("Game engine manager cleanup\n");
    gameEngine_cleanup();
    isInit = false;
}