#include "game_engine_manager.h"
#include "sensor_game_engine_manager.h"
#include "chessHelper.h"
#include "game_engine.h"
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX_FEN_STRING_LEN 128

static bool isInit  = false;
static pthread_t gameManagerThread; 

static char boadStateFenString[MAX_FEN_STRING_LEN] ;
static bool whiteCanCastleToKingSide = true;
static bool blackCanCastleToKingSide = true;
static bool whiteCanCastleToQueenSide = true;
static bool blackCanCastleToQueenSide = true;
static int totalMoves  = 1;
static Piece board[8][8];


static char *fenString[7] =  {"-", "p", "r", "n", "b", "q", "k"}; // this has to match the PieceType enum
static char returnLine[MAX_FEN_STRING_LEN] = {0};
static char from[3], to[3];

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



// got this from https://rdrr.io/github/dryguy/rbitr/src/R/board_to_fen.R
static void SensorGameEngineManager_getFenString(Piece board[8][8]) {
    int fenIndex = 0;
    memset(boadStateFenString, 0, MAX_FEN_STRING_LEN);

    // Piece placement
    for (int i = 7;i >= 0; i--) {
        int emptyCount = 0;
        
        for (int j = 0; j < 8; j++) {
            Piece currPiece = board[i][j];

            SensorGameEngineManager_kingMoveCheck(currPiece);


            if (currPiece.type == EMPTY) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    boadStateFenString[fenIndex++] = '0' + emptyCount;
                    emptyCount = 0;
                }
                char pieceChar = *fenString[currPiece.type];
                // switch(currPiece.type) {
                //     case PAWN:   pieceChar = 'p'; break;
                //     case ROOK:   pieceChar = 'r'; break;
                //     case KNIGHT: pieceChar = 'n'; break;
                //     case BISHOP: pieceChar = 'b'; break;
                //     case QUEEN:  pieceChar = 'q'; break;
                //     case KING:   pieceChar = 'k'; break;
                //     default:     pieceChar = ' ';
                // }
                boadStateFenString[fenIndex++] = 
                    (currPiece.color == WHITE) ? toupper(pieceChar) : pieceChar;
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
    boadStateFenString[fenIndex++] = getCurrentTurnString();

    // Castling availability
    boadStateFenString[fenIndex++] = ' ';
    bool anyCastle = false;


    if (whiteCanCastleToKingSide)  { boadStateFenString[fenIndex++] = 'K'; anyCastle = true; }
    if (whiteCanCastleToQueenSide) { boadStateFenString[fenIndex++] = 'Q'; anyCastle = true; }
    if (blackCanCastleToKingSide)  { boadStateFenString[fenIndex++] = 'k'; anyCastle = true; }
    if (blackCanCastleToQueenSide) { boadStateFenString[fenIndex++] = 'q'; anyCastle = true; }
    if (!anyCastle) boadStateFenString[fenIndex++] = '-';

    // En passant
    boadStateFenString[fenIndex++] = ' ';
    boadStateFenString[fenIndex++] = '-'; // maybe track this

    // Halfmove clock
    boadStateFenString[fenIndex++] = ' ';
    boadStateFenString[fenIndex++] = '0'; // maybe track this

    // Fullmove number
    boadStateFenString[fenIndex++] = ' '; // change this to have actual move numbers
    snprintf(boadStateFenString + fenIndex, MAX_FEN_STRING_LEN - fenIndex, "%d", totalMoves);
    
    boadStateFenString[MAX_FEN_STRING_LEN-1] = '\0';
}

static void Game_engine_manager_parseOutput(char *line, char *from, char *to){
    char bestmove[6];
    sscanf(line, "bestmove %4s", bestmove);

    strncpy(from, bestmove, 2); // Copy "e2"
    from[2] = '\0';

    strncpy(to, bestmove + 2, 2); // Copy "e4"
    to[2] = '\0';
}

static void * Game_engine_manager_startGameEngine(void * gameMode){
    int * game = gameMode;
    *game = 1;

    gameEngine_init();

    while(1){

        pthread_mutex_lock(&boardMutex);

        while (!isStockfishTurn) {
            pthread_cond_wait(&stockfishTurnCond, &boardMutex);
        }

        // pthread_mutex_unlock(&boardMutex);
        // pthread_mutex_lock(&boardMutex);

        copyBoardState(board);
        // printf("done copying board\n");
        // printBoard(board);
        SensorGameEngineManager_getFenString(board);

        // send updated position to the game engine
        printf("this is the fenstring\n %s\n", boadStateFenString);

        memset(returnLine, 0, MAX_FEN_STRING_LEN);
        gameEngine_sendCmd(CMD_POSITION, boadStateFenString,returnLine);
        memset(returnLine, 0, MAX_FEN_STRING_LEN);
        // get the best move
        // printf("sending go cmd\n");
        gameEngine_sendCmd(CMD_GO, NULL,returnLine);

        printf("Bestmove found by Stockfish %s\n ", returnLine);
        Game_engine_manager_parseOutput(returnLine, from, to);

        printf("from: %s, to: %s\n", from, to);
        processInput(from);
        processInput(to);

        isUserTurn = true;
        isStockfishTurn = false;

        // Signal the Stockfish thread to start
        pthread_cond_signal(&userTurnCond);
        pthread_mutex_unlock(&boardMutex);
        totalMoves += 1;
    }
    return NULL;
}


void Game_engine_manager_init(int gameMode){
    isInit = true;
    printf("init game engine manager\n");
    gameManagerThread = pthread_create(&gameManagerThread, NULL,Game_engine_manager_startGameEngine,(void *) &gameMode);

}



void Game_engine_manager_cleanup(){
    pthread_join(gameManagerThread, NULL);
}