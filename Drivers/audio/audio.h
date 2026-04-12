#ifndef AUDIO_H
#define AUDIO_H

#include "../../Core/Inc/main.h"

void audio_play_tone(uint32_t freq_hz, uint32_t duration_ms);
void audio_stop(void);

#endif
