#include "chessHelper.h"
#include "sensor_game_engine_manager.h"
<<<<<<< HEAD
#include "BoardReader.h"
#include "game_engine_manager.h"
#include <unistd.h>
=======
>>>>>>> 334f5df (Setup for led, buggy)
#include "logic_led_manager.h"


pthread_cond_t stockfishTurnCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t userTurnCond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t ledCondVar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t boardMutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t ledMutex = PTHREAD_MUTEX_INITIALIZER;

bool isStockfishTurn = false;
bool isUserTurn = true;
bool isChangeLed = false;


static Piece board[8][8];
static int possible[8][8];
static int pieceSelected = 0;
static int selectedRow = -1, selectedCol = -1;
static Color currentTurn = WHITE;
static bool gameOver = false;
static int totalMoves = 0;
static char from[3];
static char to[3];
static bool invalidMove = false;


// Initialize board with standard chess starting positions.
void initializeBoard() {
    // Set all squares to EMPTY.
    // //pthread_mutex_lock(&boardMutex);
    for (int r = 0; r < 8; r++){
        for (int c = 0; c < 8; c++){
            board[r][c].type = EMPTY;
            board[r][c].color = NONE;
            board[r][c].moved = 0;
            board[r][c].side = NONE_SIDE; // Default side.
        }
    }
    // //pthread_mutex_unlock(&boardMutex);
    // Standard setup:
    // Black pieces (placed on rows 0 and 1)
    // Row 0.
    board[0][0].type = ROOK;   board[0][0].color = WHITE; board[0][0].side = QUEEN_SIDE;
    board[0][1].type = KNIGHT; board[0][1].color = WHITE; board[0][1].side = QUEEN_SIDE;
    board[0][2].type = BISHOP; board[0][2].color = WHITE; board[0][2].side = QUEEN_SIDE;
    board[0][3].type = QUEEN;  board[0][3].color = WHITE; board[0][3].side = NONE_SIDE;
    board[0][4].type = KING;   board[0][4].color = WHITE; board[0][4].side = NONE_SIDE;
    board[0][5].type = BISHOP; board[0][5].color = WHITE; board[0][5].side = KING_SIDE;
    board[0][6].type = KNIGHT; board[0][6].color = WHITE; board[0][6].side = KING_SIDE;
    board[0][7].type = ROOK;   board[0][7].color = WHITE; board[0][7].side = KING_SIDE;
    for (int c = 0; c < 8; c++){
        board[1][c].type = PAWN;
        board[1][c].color = WHITE;
        board[1][c].side = NONE_SIDE;
    }
    // White pieces (placed on rows 6 and 7)
    for (int c = 0; c < 8; c++){
        board[6][c].type = PAWN;
        board[6][c].color = BLACK;
        board[6][c].side = NONE_SIDE;
    }
    board[7][0].type = ROOK;   board[7][0].color = BLACK; board[7][0].side = QUEEN_SIDE;
    board[7][1].type = KNIGHT; board[7][1].color = BLACK; board[7][1].side = QUEEN_SIDE;
    board[7][2].type = BISHOP; board[7][2].color = BLACK; board[7][2].side = QUEEN_SIDE;
    board[7][3].type = QUEEN;  board[7][3].color = BLACK; board[7][3].side = NONE_SIDE;
    board[7][4].type = KING;   board[7][4].color = BLACK; board[7][4].side = NONE_SIDE;
    board[7][5].type = BISHOP; board[7][5].color = BLACK; board[7][5].side = KING_SIDE;
    board[7][6].type = KNIGHT; board[7][6].color = BLACK; board[7][6].side = KING_SIDE;
    board[7][7].type = ROOK;   board[7][7].color = BLACK; board[7][7].side = KING_SIDE;
}

// Print the board state and possible moves.
void printBoard() {
    char symbol[5];
    // Print column header.
    printf("  A  B  C  D  E  F  G  H\n");

    //pthread_mutex_lock(&boardMutex);
    for (int r = 7; r >= 0; r--){
        printf("%d ", r + 1);
        for (int c = 0; c < 8; c++){
            // If a piece is selected and this square is a valid move, show a marker.
            if (pieceSelected && possible[r][c]) {
                if (board[r][c].type != EMPTY && board[r][c].color != board[selectedRow][selectedCol].color)
                    printf("✹  ");
                else
                    printf("⬤  ");
            } else {
                getPieceSymbol(board[r][c], symbol);
                printf("%s ", symbol);
            }
        }
        printf("\n");
    }
    //pthread_mutex_unlock(&boardMutex);

}

// Convert a piece to its display symbol.
void getPieceSymbol(Piece p, char *symbol) {
    if (p.type == EMPTY) {
        strcpy(symbol, ". ");
    } else if (p.type == PAWN) {
        strcpy(symbol, (p.color == WHITE ? "♟ " : "♙ "));
    } else if (p.type == ROOK) {
        strcpy(symbol, (p.color == WHITE ? "♜ " : "♖ "));
    } else if (p.type == KNIGHT) {
        strcpy(symbol, (p.color == WHITE ? "♞ " : "♘ "));
    } else if (p.type == BISHOP) {
        strcpy(symbol, (p.color == WHITE ? "♝ " : "♗ "));
    } else if (p.type == QUEEN) {
        strcpy(symbol, (p.color == WHITE ? "♛ " : "♕ "));
    } else if (p.type == KING) {
        strcpy(symbol, (p.color == WHITE ? "♚ " : "♔ "));
    }
}

// Parse an input coordinate (like "7A") into row and column indices.
int parseCoordinate(const char *input, int *row, int *col) {
    int r;
    char colChar;
    // Expect input format: a number (row) followed by a letter (column).
    if (sscanf(input, "%c%d", &colChar, &r) != 2)
        return 0;
    colChar = toupper(colChar);
    if (r < 1 || r > 8 || colChar < 'A' || colChar > 'H')
        return 0;
    *row = r - 1;
    *col = colChar - 'A';
    return 1;
}

// Calculate valid moves for the piece at board[r][c].
// (Basic moves for each piece type; special moves are omitted.)
void getPossibleMoves(int r, int c) {
    Piece piece = board[r][c];
    if (piece.type == EMPTY)
        return;
    
    // Pawn moves.
    ////pthread_mutex_lock(&boardMutex);
    if (piece.type == PAWN) {
        if (piece.color == BLACK) {
            if (r - 1 >= 0 && board[r - 1][c].type == EMPTY) {
                possible[r - 1][c] = 1;
                if (r == 6 && board[r - 2][c].type == EMPTY)
                    possible[r - 2][c] = 1;
            }
            // Diagonal captures.
            if (r - 1 >= 0 && c - 1 >= 0 &&
                board[r - 1][c - 1].type != EMPTY &&
                board[r - 1][c - 1].color == WHITE)
                possible[r - 1][c - 1] = 1;
            if (r - 1 >= 0 && c + 1 < 8 &&
                board[r - 1][c + 1].type != EMPTY &&
                board[r - 1][c + 1].color == WHITE)
                possible[r - 1][c + 1] = 1;
        } else { // BLACK pawn moves.
            if (r + 1 < 8 && board[r + 1][c].type == EMPTY) {
                possible[r + 1][c] = 1;
                if (r == 1 && board[r + 2][c].type == EMPTY)
                    possible[r + 2][c] = 1;
            }
            if (r + 1 < 8 && c - 1 >= 0 &&
                board[r + 1][c - 1].type != EMPTY &&
                board[r + 1][c - 1].color == BLACK)
                possible[r + 1][c - 1] = 1;
            if (r + 1 < 8 && c + 1 < 8 &&
                board[r + 1][c + 1].type != EMPTY &&
                board[r + 1][c + 1].color == BLACK)
                possible[r + 1][c + 1] = 1;
        }
    }
    // Knight moves.
    else if (piece.type == KNIGHT) {
        int moves[8][2] = {
            {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
            {1, -2},  {1, 2},  {2, -1},  {2, 1}
        };
        for (int i = 0; i < 8; i++) {
            int nr = r + moves[i][0], nc = c + moves[i][1];
            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY || board[nr][nc].color != piece.color)
                    possible[nr][nc] = 1;
            }
        }
    }
    // Rook moves.
    else if (piece.type == ROOK) {
        int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
        for (int i = 0; i < 4; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if (board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // Bishop moves.
    else if (piece.type == BISHOP) {
        int directions[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
        for (int i = 0; i < 4; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if (board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // Queen moves: combination of rook and bishop moves.
    else if (piece.type == QUEEN) {
        int directions[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };
        for (int i = 0; i < 8; i++){
            int dr = directions[i][0], dc = directions[i][1];
            int nr = r + dr, nc = c + dc;
            while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY) {
                    possible[nr][nc] = 1;
                } else {
                    if (board[nr][nc].color != piece.color)
                        possible[nr][nc] = 1;
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    // King moves.
    else if (piece.type == KING) {
        int moves[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };
        for (int i = 0; i < 8; i++) {
            int nr = r + moves[i][0], nc = c + moves[i][1];
            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                if (board[nr][nc].type == EMPTY || board[nr][nc].color != piece.color)
                    possible[nr][nc] = 1;
            }
        }
        // Note: Castling is not implemented in this simple version.
    }
    //pthread_mutex_unlock(&boardMutex);
}

// Return a string name for the piece type.
const char* getPieceName(Piece p) {
    switch (p.type) {
        case PAWN:   return "Pawn";
        case ROOK:   return "Rook";
        case KNIGHT: return "Knight";
        case BISHOP: return "Bishop";
        case QUEEN:  return "Queen";
        case KING:   return "King";
        default:     return "Empty";
    }
}


int validateInput(const char *input) {
    if (strchr(input, ',') != NULL) {
        printf("Error: Cannot pick two pieces at once!\n");
        return 0;
    }
    return 1;
}

// Depending on whether a piece is already selected, dispatch to the proper processor.
void processInput(const char *input) {
    if (!pieceSelected)
        processPieceSelection(input);
    else
        processDestination(input);
}

// Process input when no piece is selected (piece selection).
void processPieceSelection(const char *input) {
    int row, col;
    if (!parseCoordinate(input, &row, &col)) {
        printf("Invalid input format. Try again.\n");
        return;
    }
    //pthread_mutex_lock(&boardMutex);
    // Verify the square contains a piece of the current turn.
    if (board[row][col].type == EMPTY || board[row][col].color != currentTurn) {
        printf("Invalid selection: No %s piece at that square.\n",
               (currentTurn == WHITE ? "WHITE" : "BLACK"));
        invalidMove = true;
        return;
    }
    //pthread_mutex_unlock(&boardMutex);

    invalidMove = false;
    pieceSelected = 1;
    selectedRow = row;
    selectedCol = col;
    memset(possible, 0, sizeof(possible));
    getPossibleMoves(row, col);
}

// Process input when a piece is already selected (destination selection).
void processDestination(const char *input) {
    int destRow, destCol;
    if (!parseCoordinate(input, &destRow, &destCol)) {
        printf("Invalid input format. Try again.\n");
        return;
    }
    if (!possible[destRow][destCol]) {
        // Allow deselection if the same square is chosen.
        if (destRow == selectedRow && destCol == selectedCol) {
            pieceSelected = 0;
            memset(possible, 0, sizeof(possible));
            invalidMove = false;
            return;
        }
        printf("Invalid move, try again.\n");
        invalidMove = true;
        return;
    }

    //pthread_mutex_lock(&boardMutex);
    // Handle captures.
    if (board[destRow][destCol].type != EMPTY) {
        if (board[destRow][destCol].type == KING) {
            printf("Captured %s %s at %d%c.\n",
                   (board[destRow][destCol].color == WHITE ? "White" : "Black"),
                   getPieceName(board[destRow][destCol]),
                   destRow + 1, 'A' + destCol);
            printf("Game over: %s King has been killed.\n",
                   (board[destRow][destCol].color == WHITE ? "White" : "Black"));
            // End the thread if the king is captured.
            gameOver = true;
        } else {
            printf("Captured %s %s at %d%c.\n",
                   (board[destRow][destCol].color == WHITE ? "White" : "Black"),
                   getPieceName(board[destRow][destCol]),
                   destRow + 1, 'A' + destCol);
        }
    }
    // Move the piece.
    board[destRow][destCol] = board[selectedRow][selectedCol];
    board[selectedRow][selectedCol].type = EMPTY;
    board[selectedRow][selectedCol].color = NONE;
    board[selectedRow][selectedCol].moved = 0;
    
    // Check for pawn promotion.
    if (board[destRow][destCol].type == PAWN) {
        if ((board[destRow][destCol].color == WHITE && destRow == 0) ||
            (board[destRow][destCol].color == BLACK && destRow == 7)) {
            board[destRow][destCol].type = QUEEN;
            printf("Pawn promoted to Queen at %d%c.\n", destRow + 1, 'A' + destCol);
        }
    }

    // Clear selection and switch turn.
    pieceSelected = 0;
    memset(possible, 0, sizeof(possible));
    invalidMove = false;
    toggleCurrentTurn();
}

void boardCoordToString(int rank, int file, char* output) {
    if (rank < 0 || rank > 7 || file < 0 || file > 7) {
        sprintf(output, "??"); // invalid input
        return;
    }

    output[0] = 'A' + file;      // file: 0–7 → A–H
    output[1] = '1' + rank;      // rank: 0–7 → 1–8
    output[2] = '\0';            // null terminator
}

static void chessHelper_makeStructForLed(int arr[8][8],LIGHT_UP *out_array) {
    int idx = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (arr[i][j] == 1) {
                out_array[idx].row = i;
                out_array[idx].col = j;
                out_array[idx].colorName = COLOR_WHITE;
                idx++;
            }
        }
    }

}



void *chessGameThread(void *arg) {
    (void)arg;
    

    char input[100];

    // Initialize the board.
    initializeBoard();
    

    while (!gameOver) {

        // Display whose turn it is.
        printf("\n%s Turn\n", (currentTurn == WHITE ? "WHITE" : "BLACK"));
        // Print the board.
        printBoard();
        
        // Display piece selection status.
        if (pieceSelected)
            printf("inAir [%d%c]\n", selectedRow + 1, 'A' + selectedCol);
        else
            printf("inAir []\n");

        
        int rank, file;
        
        if(pieceSelected){
            while (1) {
                if (boardReader_detectDrop(&rank, &file)) break;
            
                // Fallback: if the square was put back, treat it as a cancel
                uint8_t state[8][8];
                boardReader_getState(state);
                if (state[selectedRow][selectedCol] == 1) {
                    printf("Move cancelled (piece returned to original square).\n");
                    pieceSelected = 0;
                    memset(possible, 0, sizeof(possible));
                    break;
                }
            
                usleep(10000); // 10 ms delay
            }

        } else{
            while(!boardReader_detectPickup(&rank, &file)){
                usleep(10000); // 10 ms delay
            }
        }
        printf("----------...\n");
        // Convert the rank and file to a string.
        boardCoordToString(rank, file, input);

        input[strcspn(input, "\n")] = 0;

        // Print the input.
        printf("Input: %s\n", input);
        
        // Validate input.
        if (!validateInput(input))
            continue;
        
        // Process the input.
        processInput(input);

        if(currentTurn == BLACK){
            Game_engine_manager_processBoardState(board, totalMoves);
            totalMoves +=1;
        }
        else {
            printf("making isChnageLed true in chessHelper\n");
            isChangeLed = true;

        }
        if(pieceSelected){
            // light up possible moves
            LIGHT_UP leds[64];
            chessHelper_makeStructForLed(possible, leds);
            LogicLedManager_changeColor(leds);

        }
        else {
            // switch all off
            LIGHT_UP *leds = NULL;
            LogicLedManager_changeColor(leds);
        }

        // check for check and check mate - this also gets the best move if a move is possible
        Game_engine_manager_processBoardState(board, totalMoves, from, to);

        if(isCheck){
            // show move by lighting up lights
            printf("Stockfish wants to make move from %s to %s\n", from, to);

        }

        if(isCheckMate){
            gameOver = true;
            // turn leds red
            
        }

        
    }
    pthread_exit(NULL);
}


//for board state
void copyBoardState(Piece dest[8][8]) {
    // printf("copying board\n");
    //pthread_mutex_lock(&boardMutex);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            dest[i][j] = board[i][j];
        }
    }
    //pthread_mutex_lock(&boardMutex);
}

//for possible array
void copyPossibleMoves(int dest[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            dest[i][j] = possible[i][j];
        }
    }
}

Color getCurrentTurn(){
    return currentTurn;
}

char getCurrentTurnString(){
    return (currentTurn == WHITE) ? 'w' : 'b';
}

void toggleCurrentTurn(){
    currentTurn = (currentTurn == WHITE ? BLACK : WHITE);
}

void chessHelper_cleanup(){}

bool chessHelper_getIsValidMove(){
    return !invalidMove;
}

