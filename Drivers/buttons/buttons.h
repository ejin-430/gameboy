#ifndef BUTTONS_H
#define BUTTONS_H

#include "../../Core/Inc/main.h"

// BTN_UP = PA10
#define BTN_UP_PORT GPIOA 
#define BTN_UP_PIN GPIO_PIN_10
#define BTN_UP 0x1

// BTN_DOWN = PB3
#define BTN_DOWN_PORT GPIOB 
#define BTN_DOWN_PIN GPIO_PIN_3
#define BTN_DOWN 0x2

// BTN_LEFT = PB4
#define BTN_LEFT_PORT GPIOB 
#define BTN_LEFT_PIN GPIO_PIN_4
#define BTN_LEFT 0x4

// BTN_RIGHT = PB10
#define BTN_RIGHT_PORT GPIOB 
#define BTN_RIGHT_PIN GPIO_PIN_10
#define BTN_RIGHT 0x8

// BTN_A = PA8
#define BTN_A_PORT GPIOA 
#define BTN_A_PIN GPIO_PIN_8
#define BTN_A 0x10

// BTN_B = PA0
#define BTN_B_PORT GPIOA
#define BTN_B_PIN GPIO_PIN_0
#define BTN_B 0x20

uint8_t buttons_read_raw(void);
void buttons_update(void);
uint8_t buttons_pressed(void);
uint8_t buttons_hold(void);

#endif
