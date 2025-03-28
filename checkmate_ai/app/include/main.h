// Define piece types and colors.
//              0      1    2      3       4       5      6
typedef enum { EMPTY, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING } PieceType;
typedef enum { NONE, WHITE, BLACK } Color;

typedef struct {
    PieceType type;
    Color color;
    int moved; // For future use (pawn first move, castling, etc.)
} Piece;

// Global variable for turn management; starting with BLACK as in your sample.
extern Color currentTurn;


// Global board: board[row][col] where row:0..7 (maps to 1..8), col:0..7 (maps to A..H)
extern Piece board[8][8];



// Function prototypes
void initializeBoard();
void printBoard();
void getPieceSymbol(Piece p, char *symbol);
int parseCoordinate(const char *input, int *row, int *col);
void getPossibleMoves(int r, int c);
const char* getPieceName(Piece p);