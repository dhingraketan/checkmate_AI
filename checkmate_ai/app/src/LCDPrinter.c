#include "LCDPrinter.h"
#include "DEV_Config.h"
#include "LCD_1in54.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include "GameController.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>  // for usleep()
#include <malloc.h>


static UWORD *s_fb = NULL;
static bool isInitialized = false;
static bool isRunning = true;
static pthread_t threadId;
static Screen_type screenType = MODE_SELECTION_SCREEN;

static void LCDPrinter_print_mode_selection_screen(){
    assert(isInitialized);

    // Check framebuffer
    if (!s_fb) {
        fprintf(stderr, "Error: s_fb is NULL in LCDPrinter_print_mode_selection_screen\n");
        return;
    }

    Paint_NewImage(s_fb, LCD_1IN54_WIDTH, LCD_1IN54_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);

    GameMode mode = GameController_getGameMode();
    const char *modeStr = (mode == ONE_V_ONE) ? "1v1" : "1vAI";

    int midY = LCD_1IN54_HEIGHT / 2;
    int textX = 10;

    Paint_DrawString_EN(textX, midY - 20, "Select Game Mode", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(textX + 90, midY + 5, modeStr, &Font20, WHITE, BLACK);

    Paint_DrawString_EN(textX, LCD_1IN54_HEIGHT - 60, "Use Joystick:", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(textX + 10, LCD_1IN54_HEIGHT - 40, "Up/Down to change", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(textX + 10, LCD_1IN54_HEIGHT - 20, "Press to Select", &Font16, WHITE, BLACK);

    LCD_1IN54_Display(s_fb);
}

static void LCDPrinter_print_result_screen() {
    assert(isInitialized);

    if (!s_fb) {
        fprintf(stderr, "Error: s_fb is NULL in LCDPrinter_print_result_screen\n");
        return;
    }

    Paint_NewImage(s_fb, LCD_1IN54_WIDTH, LCD_1IN54_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);

    Player winner = GameController_getWinner();
    const char *winnerStr = (winner == PLAYER_WHITE) ? "White Wins!" : "Black Wins!";

    int midY = LCD_1IN54_HEIGHT / 2;
    int textX = 10;

    Paint_DrawString_EN(textX, midY - 30, "Game Over!", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(textX, midY, winnerStr, &Font24, WHITE, BLACK);
    Paint_DrawString_EN(textX, midY + 30, "Congratulations!", &Font24, WHITE, BLACK);

    LCD_1IN54_Display(s_fb);
}

static void LCDPrinter_print_game_screen() {
    assert(isInitialized);

    if (!s_fb) {
        fprintf(stderr, "Error: s_fb is NULL in LCDPrinter_print_game_screen\n");
        return;
    }

    Paint_NewImage(s_fb, LCD_1IN54_WIDTH, LCD_1IN54_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);

    Game_Status status;
    GameController_getGameStatus(&status);

    int whiteMin = status.whiteTimeRemaining / 60;
    int whiteSec = status.whiteTimeRemaining % 60;
    int blackMin = status.blackTimeRemaining / 60;
    int blackSec = status.blackTimeRemaining % 60;

    char timeWhite[32], timeBlack[32];
    snprintf(timeWhite, sizeof(timeWhite), "White: %02d:%02d", whiteMin, whiteSec);
    snprintf(timeBlack, sizeof(timeBlack), "Black: %02d:%02d", blackMin, blackSec);

    const char* modeStr = (status.gameMode == ONE_V_ONE) ? "1 v 1" : "1 v AI";
    const char* turnStr = (status.turn == PLAYER_WHITE) ? "WHITE's Turn" : "BLACK's Turn";

    Paint_DrawString_EN(10, 30, timeWhite, &Font24, WHITE, BLACK);
    Paint_DrawString_EN(10, 60, timeBlack, &Font24, WHITE, BLACK);
    Paint_DrawString_EN(10, 100, modeStr, &Font24, WHITE, BLACK);
    Paint_DrawString_EN(10, 130, turnStr, &Font24, WHITE, BLACK);

    LCD_1IN54_Display(s_fb);
}

static void *lcd_printer_print(void *arg) {
    (void)arg;  
    assert(isInitialized);

    while (atomic_load(&isRunning)) {
        switch (atomic_load(&screenType)) {
            case MODE_SELECTION_SCREEN: {
                LCDPrinter_print_mode_selection_screen();
                break;
            }
            case GAME_SCREEN: {
                LCDPrinter_print_game_screen();
                break;
            }
            case RESULT_SCREEN: {
                LCDPrinter_print_result_screen();
                break;
            }
        }
        usleep(50000);
    }
    return NULL;
}

void lcd_printer_toogle_screen(Screen_type screen) {

    assert(isInitialized);

    if (screen == MODE_SELECTION_SCREEN) {
        atomic_store(&screenType, MODE_SELECTION_SCREEN);
    } else if (screen == GAME_SCREEN) {
        atomic_store(&screenType, GAME_SCREEN);
    } else if (screen == RESULT_SCREEN) {
        atomic_store(&screenType, RESULT_SCREEN);
    } else {
        fprintf(stderr, "Error: Invalid screen type\n");
        return;
    }

    
}

void lcd_printer_init() {
    assert(!isInitialized);

    if (DEV_ModuleInit() != 0) {
        DEV_ModuleExit();
        fprintf(stderr, "Error: Failed to initialize DEV module\n");
        exit(1);
    }

    DEV_Delay_ms(2000);
    LCD_1IN54_Init(HORIZONTAL);
    LCD_1IN54_Clear(WHITE);
    LCD_SetBacklight(1023);

    UDOUBLE Imagesize = LCD_1IN54_HEIGHT * LCD_1IN54_WIDTH * sizeof(UWORD);
    s_fb = (UWORD *)malloc(Imagesize);
    if (!s_fb) {
        fprintf(stderr, "Error: Failed to allocate memory for s_fb\n");
        exit(1);
    }

    isInitialized = true;

    if (pthread_create(&threadId, NULL, lcd_printer_print, NULL) != 0) {
        fprintf(stderr, "Error: Failed to create printer thread\n");
        free(s_fb);
        exit(1);
    }
}

void lcd_printer_stop_and_cleanup() {
    // // assert(isInitialized);

    atomic_store(&isRunning, false);
    pthread_join(threadId, NULL);

    if (s_fb) {
        free(s_fb);
        s_fb = NULL;
    }

    DEV_ModuleExit();
    isInitialized = false;
}