#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include "../Core/Inc/main.h"
#include "../Drivers/lcd/lcd.h"

#define LCD_WIDTH 240
#define LCD_HEIGHT 320
#define FONT_WIDTH 5 
#define FONT_HEIGHT 7
#define FONT_SPACING 1

void gfx_clear_screen(uint16_t colour);
void gfx_draw_char(uint16_t x, uint16_t y, const char c, uint16_t colour, uint16_t bg_colour);
void gfx_draw_string(uint16_t x, uint16_t y, const char * s, uint16_t colour, uint16_t bg_colour);
void gfx_draw_sprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t * sprite, uint16_t colour, uint16_t bg_colour);

#endif
