#include <sensor_game_engine_manager.h>
#include "game_engine.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include "main.h"
#include <ctype.h>

#define MAX_FEN_STRING_LEN 128
static bool isInit = false;
static char boadStateFenString[MAX_FEN_STRING_LEN] ;
static bool whiteCanCastleToKingSide = true;
static bool blackCanCastleToKingSide = true;
static bool whiteCanCastleToQueenSide = true;
static bool blackCanCastleToQueenSide = true;

static atomic_int totalMoves = 1;
// static atomic_bool canCastle = false;


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

static void SensorGameEngineManager_getFenString(Piece board[8][8]) {
    int fenIndex = 0;
    memset(boadStateFenString, 0, MAX_FEN_STRING_LEN);

    // Piece placement
    for (int i = 0; i < 8; i++) {
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
                char pieceChar = ' ';
                switch(currPiece.type) {
                    case PAWN:   pieceChar = 'p'; break;
                    case ROOK:   pieceChar = 'r'; break;
                    case KNIGHT: pieceChar = 'n'; break;
                    case BISHOP: pieceChar = 'b'; break;
                    case QUEEN:  pieceChar = 'q'; break;
                    case KING:   pieceChar = 'k'; break;
                    default:     pieceChar = ' ';
                }
                boadStateFenString[fenIndex++] = 
                    (currPiece.color == WHITE) ? toupper(pieceChar) : pieceChar;
            }
        }
        
        if (emptyCount > 0) {
            boadStateFenString[fenIndex++] = '0' + emptyCount;
        }
        
        if (i < 7) {
            boadStateFenString[fenIndex++] = '/';
        }
    }

    // Active color
    boadStateFenString[fenIndex++] = ' ';
    boadStateFenString[fenIndex++] = (currentTurn == WHITE) ? 'w' : 'b';

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





static void * SensorGameEngineManager_init(){
    isInit = true;
    // init game logic
    gameEngine_init();
    initializeBoard();
    
    while(1){
        char input[256];
        if (fgets(input, sizeof(input), stdin) != NULL) { 
            // change this to game logic
            printf("user input %s\n", input);
           

            // replace all of this with game logic code
            
            int currRow = (int) input[0];
            int destRow = (int) input[3];
            int currCol = (int) input[1];
            int destCol = (int) input[4];
            sscanf(input, "%d %d %d %d", &currRow, &currCol, &destRow, &destCol);
            // if it is a legal move then we get the board state from game logic
            board[destRow][destCol] = board[currRow][currCol];
            board[currRow][currCol].type = EMPTY;
            board[currRow][currCol].color = NONE;
            board[currRow][currCol].moved = 0;
            //
            printf("des %d %d src %d %d\n", destRow, destCol, currRow,currCol);
            for(int i = 0; i<8; i++){
                for(int j = 0; j<8;j++){
                    printf("%d ",board[i][j].type);
                }
                printf("\n");
            }

            printBoard(board);
            

            // convert this board state to a fen string 
            SensorGameEngineManager_getFenString(board);
            if(currentTurn == BLACK){
                totalMoves +=1;
            }
            // send updated position to the game engine
            printf("this is the fenstring\n %s\n", boadStateFenString);
            gameEngine_sendCmd(CMD_POSITION, boadStateFenString);
            // get the best move
            printf("sending go cmd\n");
            gameEngine_sendCmd(CMD_GO, NULL);
        }
    }
    return NULL;

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