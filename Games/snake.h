#ifndef SNAKE_H
#define SNAKE_H

#include <stdio.h>
#include "../Core/Inc/main.h"
#include "../Drivers/buttons/buttons.h"
#include "../Graphics/graphics.h"
#include "../Drivers/audio/audio.h"

#define TILE_SIZE 8
#define GRID_WIDTH 30       // 240/8
#define GRID_HEIGHT 40      // 320/8
#define MAX_SNAKE_LEN (GRID_WIDTH * GRID_HEIGHT)
typedef struct{uint16_t x; uint16_t y;} Position; // in tile coordinate, not pixels 
#define DIR_UP 0 
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3
#define SNAKE_FRAME_RATE 200      // 200ms

void snake_init(void); 
void snake_input(void);
void snake_update(void);
void spawn_food(void);
void snake_run(void);

#endif
