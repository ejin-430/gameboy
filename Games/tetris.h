#ifndef TETRIS_H
#define TETRIS_H

#include <stdio.h>
#include "../Core/Inc/main.h"
#include "../Drivers/buttons/buttons.h"
#include "../Graphics/graphics.h"
#include "../Drivers/audio/audio.h"

#define TETRIS_TILE 12
#define TETRIS_WIDTH 10
#define TETRIS_HEIGHT 20
#define FIELD_X 60      // (240 - 10*12)/2
#define FIELD_Y 10

#define PIECES 7
typedef struct {
    uint8_t type;   // I, O, T, S, Z, L J
    uint8_t rot; 
    int8_t x;
    int8_t y;
} Piece;
#define BASE_DROP_MS 500

void tetris_init(void); 
void tetris_input(void);
void tetris_update(void);
void tetris_run(void);

#endif
