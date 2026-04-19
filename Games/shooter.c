#include "shooter.h"
#include "../System/scores.h"

static Player player;
static const uint8_t shooter_sprite[] = {0xE0, 0x78, 0x3E, 0xFF, 0xFF, 0x3E, 0x78, 0xE0};
static Bullet bullets[MAX_BULLET]; 
static uint32_t last_fire_time; 
static Enemy enemies[MAX_ENEMY]; 
static uint32_t last_spawn_time;
static uint32_t spawn_interval;
static uint32_t last_enemy_move_time;


static uint16_t score;
static uint8_t game_over;

// Colour codes 
static const uint16_t shooter_bg = 0x0000;
static const uint16_t player_colour = 0xFFE0; 
static const uint16_t bullet_colour = 0xFFFF; 
static const uint16_t enemy_colour = 0xF800; 

static void draw_cell (uint8_t row, uint8_t col, uint16_t colour) {
    uint16_t x0 = col * SHOOTER_TILE;
    uint16_t y0 = row * SHOOTER_TILE;
    lcd_fill_rect(x0, y0, SHOOTER_TILE, SHOOTER_TILE, colour);
}

static void draw_enemy (int8_t row, int8_t col, uint16_t colour) {
    draw_cell(row, col, colour);
    draw_cell(row, col + 1, colour);
    draw_cell(row + 1, col, colour);
    draw_cell(row + 1, col + 1, colour);
}

static void draw_score () {
    char score_str[20]; 
    lcd_fill_rect(0, 0, LCD_WIDTH, SHOOTER_TILE, shooter_bg);
    sprintf(score_str, "Score: %d", score);
    gfx_draw_string(10, 10, score_str, 0xFFFF, shooter_bg);
}

static void collision () {
    // Bullet vs Enemy 
    for (int i = 0; i < MAX_BULLET; i++) {
        if (bullets[i].active == 1) {
            for (int j = 0; j < MAX_ENEMY; j++) {
                if (enemies[j].active == 1 &&
                    bullets[i].x >= enemies[j].x && bullets[i].x <= enemies[j].x + 1 &&
                    bullets[i].y >= enemies[j].y && bullets[i].y <= enemies[j].y + 1) {
                    score += 100;
                    draw_cell(bullets[i].y, bullets[i].x, shooter_bg);
                    draw_enemy(enemies[j].y, enemies[j].x, shooter_bg);
                    bullets[i].active = 0; 
                    enemies[j].active = 0; 

                    spawn_interval -= 50; 
                    if (spawn_interval < 500) spawn_interval = 500;

                    audio_play_tone(1300, 30);
                    draw_score();
                }
            }
        }
    }

    // Enemy vs Player
    for (int i = 0; i < MAX_ENEMY; i++) {
        if (enemies[i].active == 1 &&
            player.x >= enemies[i].x && player.x <= enemies[i].x + 1 &&
            player.y >= enemies[i].y && player.y <= enemies[i].y + 1) {
            game_over = 1; 
            break;
        }
    }
}

void shooter_init() {
    // Update constants 
    score = 0; 
    game_over = 0;
    last_fire_time = 0;
    last_spawn_time = 0;
    spawn_interval = 3000;
    last_enemy_move_time = 0;

    // Update elements 
    player.x = SHOOTER_WIDTH/2; 
    player.y = SHOOTER_HEIGHT-1; 
    for (int i = 0; i < MAX_BULLET; i++) bullets[i].active = 0; 
    for (int i = 0; i < MAX_ENEMY; i++) enemies[i].active = 0; 

    // Draw initial state
    gfx_clear_screen(shooter_bg);
    draw_score();
    gfx_draw_sprite(player.x * SHOOTER_TILE, player.y * SHOOTER_TILE, SHOOTER_TILE, SHOOTER_TILE, shooter_sprite, player_colour, shooter_bg);
}

void shooter_input() {
    // Left/Right using press
    uint8_t pressed = buttons_pressed(); 
    if ((pressed & BTN_LEFT) && (player.x > 0)) {
        gfx_draw_sprite(player.x * SHOOTER_TILE, player.y * SHOOTER_TILE, SHOOTER_TILE, SHOOTER_TILE, shooter_sprite, shooter_bg, shooter_bg);
        player.x = player.x - 1; 
        gfx_draw_sprite(player.x * SHOOTER_TILE, player.y * SHOOTER_TILE, SHOOTER_TILE, SHOOTER_TILE, shooter_sprite, player_colour, shooter_bg);
    } else if ((pressed & BTN_RIGHT) && (player.x < SHOOTER_WIDTH-1)) {
        gfx_draw_sprite(player.x * SHOOTER_TILE, player.y * SHOOTER_TILE, SHOOTER_TILE, SHOOTER_TILE, shooter_sprite, shooter_bg, shooter_bg);
        player.x = player.x + 1; 
        gfx_draw_sprite(player.x * SHOOTER_TILE, player.y * SHOOTER_TILE, SHOOTER_TILE, SHOOTER_TILE, shooter_sprite, player_colour, shooter_bg);
    }

    // Hold to shoot 
    uint8_t hold = buttons_hold(); 
    if ((hold & BTN_A) && (HAL_GetTick() - last_fire_time > 100)) {
        for (int i = 0; i < MAX_BULLET; i++) {
            if (bullets[i].active == 0) {
                bullets[i].active = 1; 
                bullets[i].x = player.x; 
                bullets[i].y = player.y - 1; 
                draw_cell(bullets[i].y, bullets[i].x, bullet_colour);
                last_fire_time = HAL_GetTick();
                audio_play_tone(2000, 30);
                break;
            }
        }
    }
}

void shooter_update() {
    // Move bullets upward 
    for (int i = 0; i < MAX_BULLET; i++) {
        if (bullets[i].active == 1) {
            draw_cell(bullets[i].y, bullets[i].x, shooter_bg);
            bullets[i].y = bullets[i].y - 1; 
            if (bullets[i].y < 1) bullets[i].active = 0;
            else draw_cell(bullets[i].y, bullets[i].x, bullet_colour);
        }
    }

    // Move enemies down
    if (HAL_GetTick() - last_enemy_move_time > 400) {
        for (int i = 0; i < MAX_ENEMY; i++) {
            if (enemies[i].active == 1) {
                draw_enemy(enemies[i].y, enemies[i].x, shooter_bg);
                enemies[i].y = enemies[i].y + 1;
                if (enemies[i].y + 1 >= SHOOTER_HEIGHT) game_over = 1;
                else draw_enemy(enemies[i].y, enemies[i].x, enemy_colour);
            }
        }
        last_enemy_move_time = HAL_GetTick();
    }

    // Spawn new enemy
    if (HAL_GetTick() - last_spawn_time > spawn_interval) {
        for (int i = 0; i < MAX_ENEMY; i++) {
            if (enemies[i].active == 0) {
                enemies[i].active = 1;
                enemies[i].x = HAL_GetTick() % (SHOOTER_WIDTH - 1);
                enemies[i].y = 1;
                draw_enemy(enemies[i].y, enemies[i].x, enemy_colour);
                last_spawn_time = HAL_GetTick();
                break;
            }
        }
    }

    collision();
}

void shooter_run() {
    // Clear stale button state
    HAL_Delay(200);
    buttons_update();
    buttons_update();

    shooter_init();

    uint32_t last_update_time = HAL_GetTick();
    while (!game_over) {
        buttons_update(); 
        shooter_input(); 
        if (HAL_GetTick() - last_update_time > SHOOTER_FRAME_RATE) {
            shooter_update(); 
            last_update_time = HAL_GetTick();
        }
    }

    gfx_clear_screen(shooter_bg);
    gfx_draw_string(10, 10, "GAME OVER ...", 0xFFFF, shooter_bg);
    char score_str[20];
    sprintf(score_str, "Score: %d", score);
    gfx_draw_string(10, 40, score_str, 0xFFFF, shooter_bg);
    if (score > scores_get(TOP_DOWN_SHOOTER)) {
        scores_set(TOP_DOWN_SHOOTER, score); 
        scores_save(); 
        gfx_draw_string(10, 70, "NEW HIGH SCORE!", 0xF800, shooter_bg);
    } else {
        sprintf(score_str, "Best score: %d", scores_get(TOP_DOWN_SHOOTER));
        gfx_draw_string(10, 70, score_str, 0xFFFF, shooter_bg);
    }

    // Wait for any buttons to be released, then wait for B press to exit
    HAL_Delay(500);
    while (1) {
        buttons_update();
        if (buttons_pressed() & BTN_B) break;
    }
}