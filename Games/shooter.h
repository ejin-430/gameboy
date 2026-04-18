#ifndef SHOOTER_H
#define SHOOTER_H

#include <stdio.h>
#include "../Core/Inc/main.h"
#include "../Drivers/buttons/buttons.h"
#include "../Graphics/graphics.h"
#include "../Drivers/audio/audio.h"

#define SHOOTER_TILE 8
#define SHOOTER_WIDTH 30
#define SHOOTER_HEIGHT 40

typedef struct {
    int8_t x;
    int8_t y;
} Player;
typedef struct {
    int8_t x;
    int8_t y;
    int8_t active;
} Bullet;
typedef struct {
    int8_t x;
    int8_t y;
    int8_t active;
} Enemy;
#define MAX_ENEMY 8
#define MAX_BULLET 10
#define SHOOTER_FRAME_RATE 200

void shooter_init(void); 
void shooter_input(void);
void shooter_update(void);
void shooter_run(void);

#endif
