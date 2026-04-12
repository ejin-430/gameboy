#ifndef MENU_H
#define MENU_H

#include "../Core/Inc/main.h"
#include "../Graphics/graphics.h"
#include "../Drivers/buttons/buttons.h"
#include "../Drivers/audio/audio.h"

// Game options
#define SNAKE 0
#define TETRIS 1
#define TOP_DOWN_SHOOTER 2

void menu_init(void); 
int menu_update(void);

#endif
