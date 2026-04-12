#include "snake.h"

static Position body[MAX_SNAKE_LEN];
static uint16_t head; 
static uint16_t tail; 
static uint16_t length;
static uint8_t direction;

static Position food;
static uint16_t score;
static uint8_t game_over;

// Colour codes 
const uint16_t bg = 0xFFFF;
const uint16_t snake_colour = 0x07E0;
const uint16_t food_colour = 0xF800;


void snake_init() {
    // Start with a snake with size 3, heading right 
    head = 2; 
    tail = 0; 
    length = 3; 
    // Positions for snake body
    body[0] = (Position){9, 20};
    body[1] = (Position){10, 20};
    body[2] = (Position){11, 20};
    direction = DIR_RIGHT;

    // Update constants 
    score = 0; 
    game_over = 0;

    // Place food 
    food = (Position){3, 4};

    // Draw initial state
    gfx_clear_screen(bg);
    // Draw snake 
    for (uint16_t i = tail; i <= head; i++) {
        lcd_fill_rect(body[i].x * TILE_SIZE, body[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE, snake_colour);
    }
    // Draw food 
    lcd_fill_rect(food.x * TILE_SIZE, food.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, food_colour);
}

void snake_input() {
    uint8_t pressed = buttons_pressed(); 

    // Block reverse directions 
    if (pressed == BTN_UP && direction != DIR_DOWN) direction = DIR_UP;
    else if (pressed == BTN_DOWN && direction != DIR_UP) direction = DIR_DOWN;
    else if (pressed == BTN_LEFT && direction != DIR_RIGHT) direction = DIR_LEFT;
    else if (pressed == BTN_RIGHT && direction != DIR_LEFT) direction = DIR_RIGHT;
}

// Random position generator for food 
void spawn_food() {
    uint8_t collision;
    food.x = HAL_GetTick() % GRID_WIDTH;
    food.y = (HAL_GetTick() / 7) % GRID_HEIGHT;

    // check for collision
    do {
        collision = 0;
        uint16_t i = tail; 
        while (i != head) {
            if (body[i].x == food.x && body[i].y == food.y) collision = 1;
            i = (i+1) % MAX_SNAKE_LEN;
        }
        if (collision) {
            food.x = (food.x + 1) % GRID_WIDTH;
            if (food.x == 0) food.y = (food.y + 1) % GRID_HEIGHT;
        }
    } while (collision);

    // draw food
    lcd_fill_rect(food.x * TILE_SIZE, food.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, food_colour);
}

void snake_update() {
    // Update head position 
    Position new_pos = body[head];
    if (direction == DIR_UP) {
        new_pos = (Position){body[head].x, body[head].y-1};
    } else if (direction == DIR_DOWN) {
        new_pos = (Position){body[head].x, body[head].y+1};
    } else if (direction == DIR_LEFT) {
        new_pos = (Position){body[head].x-1, body[head].y};
    } else if (direction == DIR_RIGHT) {
        new_pos = (Position){body[head].x+1, body[head].y};
    }
    head = (head+1) % MAX_SNAKE_LEN;
    body[head] = new_pos;

    // Wall collision check 
    if (new_pos.x >= GRID_WIDTH || new_pos.y >= GRID_HEIGHT) {
        game_over = 1;
        return;
    }
    // Self collision check 
    uint16_t i = tail; 
    while (i != head) {
        if (body[i].x == new_pos.x && body[i].y == new_pos.y) {
            game_over = 1;
            return;
        }
        i = (i+1) % MAX_SNAKE_LEN;
    }

    // Eating food? 
    if (new_pos.x == food.x && new_pos.y == food.y) {
        score++; 
        spawn_food(); 
    } else {
        lcd_fill_rect(body[tail].x * TILE_SIZE, body[tail].y * TILE_SIZE, TILE_SIZE, TILE_SIZE, bg);
        tail = (tail + 1) % MAX_SNAKE_LEN;
    }

    // add new head
    lcd_fill_rect(body[head].x * TILE_SIZE, body[head].y * TILE_SIZE, TILE_SIZE, TILE_SIZE, snake_colour);
}

void snake_run() {
    // Clear stale button state
    HAL_Delay(200);
    buttons_update();
    buttons_update();

    snake_init();

    uint32_t last_update_time = HAL_GetTick();
    while (!game_over) {
        buttons_update(); 
        snake_input(); 
        if (HAL_GetTick() - last_update_time > SNAKE_FRAME_RATE) {
            snake_update(); 
            last_update_time = HAL_GetTick();
        }
    }

    gfx_clear_screen(bg);
    gfx_draw_string(10, 10, "GAME OVER ...", 0x0000, bg);
    char score_str[20];
    sprintf(score_str, "Score: %d", score);
    gfx_draw_string(10, 40, score_str, 0x0000, bg);

    // Wait for any buttons to be released, then wait for B press to exit
    HAL_Delay(500);
    while (1) {
        buttons_update();
        if (buttons_pressed() & BTN_B) break;
    }
}