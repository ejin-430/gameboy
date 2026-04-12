#include "lcd.h"

extern SPI_HandleTypeDef hspi1;

// Toggle control pins
void sel_lcd(){
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, 0);
}

void desel_lcd(){
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, 1);
}

void cmd_mode(){
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, 0);
}

void data_mode(){
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, 1);
}

// Send a command byte (CS stays LOW after — caller must deselect when done)
void lcd_write_cmd(uint8_t cmd){
    desel_lcd();         // Ensure clean CS transition
    cmd_mode();          // DC = 0 (command)
    sel_lcd();           // CS = 0 (begin transaction)
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    // NOTE: CS stays LOW so subsequent data bytes are part of this command
    data_mode();         // Switch to data mode for parameter bytes that follow
}

// Send a data byte (CS must already be LOW from a preceding command)
void lcd_write_data(uint8_t data){
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
}

// Send a standalone command with no parameters
void lcd_write_cmd_only(uint8_t cmd){
    lcd_write_cmd(cmd);
    desel_lcd();
}


void lcd_init(){
    // Hardware reset
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, 0);
    HAL_Delay(20);
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, 1);
    HAL_Delay(150);

    // Software reset
    lcd_write_cmd_only(0x01);
    HAL_Delay(150);

    // --- Undocumented but recommended init commands (from Adafruit library) ---
    lcd_write_cmd(0xEF);
    lcd_write_data(0x03);
    lcd_write_data(0x80);
    lcd_write_data(0x02);
    desel_lcd();

    // Power control A
    lcd_write_cmd(0xCB);
    lcd_write_data(0x39);
    lcd_write_data(0x2C);
    lcd_write_data(0x00);
    lcd_write_data(0x34);
    lcd_write_data(0x02);
    desel_lcd();

    // Power control B
    lcd_write_cmd(0xCF);
    lcd_write_data(0x00);
    lcd_write_data(0xC1);
    lcd_write_data(0x30);
    desel_lcd();

    // Power on sequence control
    lcd_write_cmd(0xED);
    lcd_write_data(0x64);
    lcd_write_data(0x03);
    lcd_write_data(0x12);
    lcd_write_data(0x81);
    desel_lcd();

    // Driver timing control A
    lcd_write_cmd(0xE8);
    lcd_write_data(0x85);
    lcd_write_data(0x00);
    lcd_write_data(0x78);
    desel_lcd();

    // Pump ratio control
    lcd_write_cmd(0xF7);
    lcd_write_data(0x20);
    desel_lcd();

    // Driver timing control B
    lcd_write_cmd(0xEA);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    desel_lcd();

    // Power control 1
    lcd_write_cmd(0xC0);
    lcd_write_data(0x23);
    desel_lcd();

    // Power control 2
    lcd_write_cmd(0xC1);
    lcd_write_data(0x10);
    desel_lcd();

    // VCOM control 1
    lcd_write_cmd(0xC5);
    lcd_write_data(0x3E);
    lcd_write_data(0x28);
    desel_lcd();

    // VCOM control 2
    lcd_write_cmd(0xC7);
    lcd_write_data(0x86);
    desel_lcd();

    // Memory access control
    lcd_write_cmd(0x36);
    lcd_write_data(0x48);
    desel_lcd();

    // Vertical scroll start address (2 bytes: high + low)
    lcd_write_cmd(0x37);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    desel_lcd();

    // Pixel format - 16 bit
    lcd_write_cmd(0x3A);
    lcd_write_data(0x55);
    desel_lcd();

    // Frame rate control
    lcd_write_cmd(0xB1);
    lcd_write_data(0x00);
    lcd_write_data(0x18);
    desel_lcd();

    // Display function control
    lcd_write_cmd(0xB6);
    lcd_write_data(0x08);
    lcd_write_data(0x82);
    lcd_write_data(0x27);
    desel_lcd();

    // 3Gamma function disable
    lcd_write_cmd(0xF2);
    lcd_write_data(0x00);
    desel_lcd();

    // Gamma curve selected
    lcd_write_cmd(0x26);
    lcd_write_data(0x01);
    desel_lcd();

    // Positive gamma correction
    lcd_write_cmd(0xE0);
    lcd_write_data(0x0F);
    lcd_write_data(0x31);
    lcd_write_data(0x2B);
    lcd_write_data(0x0C);
    lcd_write_data(0x0E);
    lcd_write_data(0x08);
    lcd_write_data(0x4E);
    lcd_write_data(0xF1);
    lcd_write_data(0x37);
    lcd_write_data(0x07);
    lcd_write_data(0x10);
    lcd_write_data(0x03);
    lcd_write_data(0x0E);
    lcd_write_data(0x09);
    lcd_write_data(0x00);
    desel_lcd();

    // Negative gamma correction
    lcd_write_cmd(0xE1);
    lcd_write_data(0x00);
    lcd_write_data(0x0E);
    lcd_write_data(0x14);
    lcd_write_data(0x03);
    lcd_write_data(0x11);
    lcd_write_data(0x07);
    lcd_write_data(0x31);
    lcd_write_data(0xC1);
    lcd_write_data(0x48);
    lcd_write_data(0x08);
    lcd_write_data(0x0F);
    lcd_write_data(0x0C);
    lcd_write_data(0x31);
    lcd_write_data(0x36);
    lcd_write_data(0x0F);
    desel_lcd();

    // Exit sleep mode
    lcd_write_cmd_only(0x11);
    HAL_Delay(150);

    // Display ON
    lcd_write_cmd_only(0x29);
    HAL_Delay(150);

    // Normal display mode
    lcd_write_cmd_only(0x13);
}

void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Define column range
    lcd_write_cmd(0x2A);
    lcd_write_data(x0 >> 8);
    lcd_write_data(x0 & 0xFF);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1 & 0xFF);
    desel_lcd();

    // Define row range
    lcd_write_cmd(0x2B);
    lcd_write_data(y0 >> 8);
    lcd_write_data(y0 & 0xFF);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1 & 0xFF);
    desel_lcd();

    // Memory write - CS stays LOW for the pixel data that follows
    lcd_write_cmd(0x2C);
}

void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t width, uint16_t
height, uint16_t colour) {
    lcd_set_window(x0, y0, x0 + width - 1, y0 + height - 1);

    for (uint16_t i = 0; i < height; i++) {
        for (uint16_t j = 0; j < width; j++) {
            lcd_write_data(colour >> 8);
            lcd_write_data(colour & 0xFF);
        }
    }
    desel_lcd();
}

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t colour) {
    lcd_set_window(x, y, x, y);

    lcd_write_data(colour >> 8);
    lcd_write_data(colour & 0xFF);
    desel_lcd();
}
