#include <stdio.h>
#include <unistd.h>
#include "GameController.h"
#include "LCDPrinter.h"
#include "BoardReader.h"
#include "logic_led_manager.h"
#include "game_engine_manager.h"
#include "joystick_help.h"

int main(){
    lcd_printer_init();
    // ADDED:
    LogicLedManager_init();
    Game_engine_manager_init();
    boardReader_init();
    joystick_help_init();

    GameController_init();
    GameController_cleanup();
    boardReader_cleanup();


    // ADDED:
    joystick_help_cleanup();
    Game_engine_manager_cleanup();
    LogicLedManager_cleanup();
    lcd_printer_stop_and_cleanup();
}
