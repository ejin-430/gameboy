#ifndef SCORES_H
#define SCORES_H

#include "../Core/Inc/main.h"
#include "menu.h"

#define NUM_GAMES 3

uint16_t scores_get(uint8_t game_id); 
void scores_set(uint8_t game_id, uint16_t val);
void scores_load(void); 
void scores_save(void);

#endif
