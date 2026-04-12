#include "audio.h"

extern TIM_HandleTypeDef htim4;

void audio_stop() {
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
}

void audio_play_tone(uint32_t freq_hz, uint32_t duration_ms) {
    // ARR: auto reload register
    // clk is 84 MHz, we need to convert it 
    uint32_t arr = (84000000 / freq_hz) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim4, arr);                  // Set the ARR
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, arr/2);    // Set the pulse width to 50% 
                                                            // so it's a square wave 
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);               // Start the PWM
    HAL_Delay(duration_ms);                                 // Wait for duration
    audio_stop();
}