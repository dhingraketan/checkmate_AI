#include "BoardReader.h"
#include "port_extender.h"
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

static pthread_t polling_thread;
static bool isRunning = false;

static uint8_t board_state[8][8];
static uint8_t previous_state[8][8];
static pthread_mutex_t board_lock = PTHREAD_MUTEX_INITIALIZER;

static bool pieceLifted = false;
static int lastPickupRank = -1;
static int lastPickupFile = -1;

static void* polling_loop() {
    while (isRunning) {
        uint8_t new_state[8][8];
        portExtender_readAllPins(new_state);

        pthread_mutex_lock(&board_lock);
        memcpy(previous_state, board_state, sizeof(board_state));
        memcpy(board_state, new_state, sizeof(board_state));
        pthread_mutex_unlock(&board_lock);

        usleep(50000); // 50ms poll interval
    }
    return NULL;
}

static bool boardReader_checkInitialPosition() {
    pthread_mutex_lock(&board_lock);

    // Check pawns
    for (int f = 0; f < 8; f++) {
        if (board_state[1][f] != 1 || board_state[6][f] != 1) {
            pthread_mutex_unlock(&board_lock);
            return false;
        }
    }

    // Check major pieces (ranks 0 and 7)
    int expected[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    for (int f = 0; f < 8; f++) {
        if (board_state[0][f] != expected[f] || board_state[7][f] != expected[f]) {
            pthread_mutex_unlock(&board_lock);
            return false;
        }
    }

    // Check empty middle rows
    for (int r = 2; r <= 5; r++) {
        for (int f = 0; f < 8; f++) {
            if (board_state[r][f] != 0) {
                pthread_mutex_unlock(&board_lock);
                return false;
            }
        }
    }

    pthread_mutex_unlock(&board_lock);
    return true;
}

void boardReader_startPolling() {
    isRunning = true;
    pthread_create(&polling_thread, NULL, polling_loop, NULL);
}


void boardReader_init() {
    portExtender_init_and_config();
    memset(board_state, 0, sizeof(board_state));
    memset(previous_state, 0, sizeof(previous_state));
    pieceLifted = false;
    lastPickupRank = -1;
    lastPickupFile = -1;

    // Read initial board state
    portExtender_readAllPins(board_state);
    boardReader_startPolling();

    // Check if the board is in the initial position
    if(!boardReader_checkInitialPosition()) {
        printf("Board is not in the initial position.\n");

        while (!boardReader_checkInitialPosition()){};
    }

    printf("Board is in the initial position.\n");

}

void boardReader_cleanup() {
    isRunning = false;
    pthread_join(polling_thread, NULL);
    portExtender_cleanup();
}

void boardReader_getState(uint8_t state[8][8]) {
    pthread_mutex_lock(&board_lock);
    memcpy(state, board_state, sizeof(board_state));
    pthread_mutex_unlock(&board_lock);
}

bool boardReader_detectPickup(int* rank, int* file) {
    pthread_mutex_lock(&board_lock);
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            if (previous_state[r][f] == 1 && board_state[r][f] == 0) {
                *rank = r;
                *file = f;
                lastPickupRank = r;
                lastPickupFile = f;
                pieceLifted = true;
                pthread_mutex_unlock(&board_lock);
                return true;
            }
        }
    }
    pthread_mutex_unlock(&board_lock);
    return false;
}

bool boardReader_detectDrop(int* rank, int* file) {
    if (!pieceLifted) return false;

    pthread_mutex_lock(&board_lock);
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            // Detect drop as any 0 → 1 transition
            if (previous_state[r][f] == 0 && board_state[r][f] == 1) {
                *rank = r;
                *file = f;
                pieceLifted = false;
                pthread_mutex_unlock(&board_lock);
                return true;
            }
        }
    }
    pthread_mutex_unlock(&board_lock);
    return false;
}

bool boardReader_detectChange(int* rank, int* file) {
    static uint8_t stable_state[8][8];
    static bool initialized = false;

    pthread_mutex_lock(&board_lock);

    if (!initialized) {
        memcpy(stable_state, board_state, sizeof(stable_state));
        initialized = true;
        pthread_mutex_unlock(&board_lock);
        return false;
    }

    // Wait for board to stabilize
    static int stable_count = 0;
    bool changed = false;
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            if (board_state[r][f] != stable_state[r][f]) {
                changed = true;
                stable_count = 0;
                memcpy(stable_state, board_state, sizeof(stable_state));
                break;
            }
        }
        if (changed) break;
    }

    if (!changed) {
        stable_count++;
        if (stable_count >= 3) { // Must be stable for 3 consecutive checks
            for (int r = 0; r < 8; r++) {
                for (int f = 0; f < 8; f++) {
                    if (board_state[r][f] != previous_state[r][f]) {
                        *rank = r;
                        *file = f;
                        pthread_mutex_unlock(&board_lock);
                        return true;
                    }
                }
            }
        }
    }

    pthread_mutex_unlock(&board_lock);
    return false;
}
