#ifndef _LCD_PRINTER_H_
#define _LCD_PRINTER_H_

typedef enum{
    MODE_SELECTION_SCREEN,
    GAME_SCREEN,
    RESULT_SCREEN
} Screen_type;

void lcd_printer_init();
void lcd_printer_toogle_screen(Screen_type screen);
void lcd_printer_stop_and_cleanup();

#endif