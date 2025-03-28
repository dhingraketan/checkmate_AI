#include <pthread.h>
#include "hal/chessHelper.h"

int main() {
    pthread_t gameThread;

    // Piece currentBoard[8][8];
    // copyBoardState(currentBoard);                 use this to get current state of the board :)


    pthread_create(&gameThread, NULL, chessGameThread, NULL);
    
    // Wait for the game thread to finish, it finishes when king dies :(
    pthread_join(gameThread, NULL);
    
    return 0;
}
