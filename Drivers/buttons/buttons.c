#include "buttons.h"

// Variables for debouncing 
static uint8_t btn_cur; 
static uint8_t btn_prev; 
static uint8_t btn_raw_last; 
static uint32_t btn_stable_time; 

// Read button inputs 
uint8_t buttons_read_raw() {
    uint8_t result = 0; 
    if (HAL_GPIO_ReadPin(BTN_UP_PORT, BTN_UP_PIN) == GPIO_PIN_RESET) result |= BTN_UP;
    if (HAL_GPIO_ReadPin(BTN_DOWN_PORT, BTN_DOWN_PIN) == GPIO_PIN_RESET) result |= BTN_DOWN;
    if (HAL_GPIO_ReadPin(BTN_LEFT_PORT, BTN_LEFT_PIN) == GPIO_PIN_RESET) result |= BTN_LEFT;
    if (HAL_GPIO_ReadPin(BTN_RIGHT_PORT, BTN_RIGHT_PIN) == GPIO_PIN_RESET) result |= BTN_RIGHT;
    if (HAL_GPIO_ReadPin(BTN_A_PORT, BTN_A_PIN) == GPIO_PIN_RESET) result |= BTN_A;
    if (HAL_GPIO_ReadPin(BTN_B_PORT, BTN_B_PIN) == GPIO_PIN_RESET) result |= BTN_B;

    return result;
}

// Debouncing logic 
void buttons_update() {
    uint8_t raw = buttons_read_raw();
    if (raw != btn_raw_last) {
        btn_stable_time = HAL_GetTick();
    } else if (HAL_GetTick() - btn_stable_time >= 20) {
        // Update if stable for more than 20ms
        btn_prev = btn_cur;
        btn_cur = raw;
    }
    btn_raw_last = raw;
}

// Find buttons that are ON now but OFF before
uint8_t buttons_pressed() {
    uint8_t press = btn_cur & (~btn_prev);
    btn_prev = btn_cur; 
    return press;
}

// Find buttons that are ON now and ON before
uint8_t buttons_hold() {
    return btn_cur & btn_prev;
}