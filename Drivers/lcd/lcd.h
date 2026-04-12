#ifndef LCD_H
#define LCD_H

#include "main.h"

// LCD_CS = D10 = PB6
#define LCD_CS_PORT GPIOB
#define LCD_CS_PIN GPIO_PIN_6

// LCD_DC = D9 = PC7
#define LCD_DC_PORT GPIOC
#define LCD_DC_PIN GPIO_PIN_7

// LCD_RST = D8 = PA9
#define LCD_RST_PORT GPIOA
#define LCD_RST_PIN GPIO_PIN_9

void lcd_init(void);
void lcd_write_cmd(uint8_t cmd);
void lcd_write_data(uint8_t data);
void lcd_write_cmd_only(uint8_t cmd);
void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t width, uint16_t
height, uint16_t colour);
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t colour);
void sel_lcd(void);
void desel_lcd(void);

#endif
