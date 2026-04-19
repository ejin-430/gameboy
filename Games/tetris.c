#include "tetris.h"
#include "../System/scores.h"

static const int8_t tetrominoes[7][4][4][2] = {
    // 0: I
    {
        {{0,0},{0,1},{0,2},{0,3}},
        {{0,0},{1,0},{2,0},{3,0}},
        {{0,0},{0,1},{0,2},{0,3}},
        {{0,0},{1,0},{2,0},{3,0}}
    },
    // 1: O
    {
        {{0,0},{0,1},{1,0},{1,1}},
        {{0,0},{0,1},{1,0},{1,1}},
        {{0,0},{0,1},{1,0},{1,1}},
        {{0,0},{0,1},{1,0},{1,1}}
    },
    // 2: T
    {
        {{0,1},{1,0},{1,1},{1,2}},
        {{0,0},{1,0},{1,1},{2,0}},
        {{0,0},{0,1},{0,2},{1,1}},
        {{0,1},{1,0},{1,1},{2,1}}
    },
    // 3: S
    {
        {{0,1},{0,2},{1,0},{1,1}},
        {{0,0},{1,0},{1,1},{2,1}},
        {{0,1},{0,2},{1,0},{1,1}},
        {{0,0},{1,0},{1,1},{2,1}}
    },
    // 4: Z
    {
        {{0,0},{0,1},{1,1},{1,2}},
        {{0,1},{1,0},{1,1},{2,0}},
        {{0,0},{0,1},{1,1},{1,2}},
        {{0,1},{1,0},{1,1},{2,0}}
    },
    // 5: L
    {
        {{0,2},{1,0},{1,1},{1,2}},
        {{0,0},{1,0},{2,0},{2,1}},
        {{0,0},{0,1},{0,2},{1,0}},
        {{0,0},{0,1},{1,1},{2,1}}
    },
    // 6: J
    {
        {{0,0},{1,0},{1,1},{1,2}},
        {{0,0},{0,1},{1,0},{2,0}},
        {{0,0},{0,1},{0,2},{1,2}},
        {{0,1},{1,1},{2,0},{2,1}}
    }
};

static const uint16_t piece_colours[7] = {
    0x07FF,  // I - cyan
    0xFFE0,  // O - yellow
    0xF81F,  // T - magenta
    0x07E0,  // S - green
    0xF800,  // Z - red
    0xFD20,  // L - orange
    0x001F   // J - blue
};

static uint16_t grid[TETRIS_HEIGHT][TETRIS_WIDTH];

static Piece current;
static uint8_t next_type;
static int8_t hold_type;      // -1 = empty
static uint8_t hold_used;     // can only hold once per drop

static uint16_t score;
static uint16_t lines;
static uint8_t level;
static uint8_t game_over;
static uint32_t drop_interval;

// Colors
static const uint16_t tetris_bg = 0x0000;       // black
static const uint16_t tetris_border = 0xC618;   // gray

// Sidebar positions (pixels)
#define PREVIEW_TILE 10
#define HOLD_X 5
#define HOLD_Y 30
#define NEXT_X 185
#define NEXT_Y 30

// Rendering helper functions 
static void draw_cell (uint8_t row, uint8_t col, uint16_t colour) {
    uint16_t x0 = FIELD_X + col * TETRIS_TILE; 
    uint16_t y0 = FIELD_Y + row * TETRIS_TILE; 
    lcd_fill_rect(x0, y0, TETRIS_TILE, TETRIS_TILE, colour);
}

static void draw_preview (int8_t type, uint16_t px, uint16_t py, uint16_t colour) {
    // Clear the 4x4 preview area first
    lcd_fill_rect(px, py, PREVIEW_TILE * 4, PREVIEW_TILE * 4, tetris_bg);
    if (type < 0) return;  // empty hold slot
    for (int i = 0; i < 4; i++) {
        uint8_t r = tetrominoes[type][0][i][0];
        uint8_t c = tetrominoes[type][0][i][1];
        lcd_fill_rect(px + c * PREVIEW_TILE, py + r * PREVIEW_TILE,
                       PREVIEW_TILE, PREVIEW_TILE, colour);
    }
}

static void draw_sidebar (void) {
    gfx_draw_string(HOLD_X, HOLD_Y - 15, "HOLD", 0xFFFF, tetris_bg);
    if (hold_type >= 0) draw_preview(hold_type, HOLD_X, HOLD_Y, piece_colours[hold_type]);
    else draw_preview(hold_type, HOLD_X, HOLD_Y, tetris_bg);

    gfx_draw_string(NEXT_X, NEXT_Y - 15, "NEXT", 0xFFFF, tetris_bg);
    draw_preview(next_type, NEXT_X, NEXT_Y, piece_colours[next_type]);
}

static void draw_piece (Piece p, uint16_t colour) {
    for (int i = 0; i < 4; i++) {
        int8_t row = tetrominoes[p.type][p.rot][i][0] + p.y; 
        int8_t col = tetrominoes[p.type][p.rot][i][1] + p.x; 
        if (row >= 0) draw_cell(row, col, colour);
    }
}

static void lock_piece (Piece p, uint16_t colour) {
    for (int i = 0; i < 4; i++) {
        int8_t row = tetrominoes[p.type][p.rot][i][0] + p.y; 
        int8_t col = tetrominoes[p.type][p.rot][i][1] + p.x; 
        if (row >= 0) {
            draw_cell(row, col, colour);
            grid[row][col] = piece_colours[p.type];
        }
    }
}

static void clear_lines () {
    int rows_cleared = 0; 
    for (int i = TETRIS_HEIGHT-1; i > 0; i--) {
        // check if current row is full
        int is_full = 1;
        for (int j = 0; j < TETRIS_WIDTH; j++) {
            if (grid[i][j] == 0) is_full = 0; 
        }

        // if is_full, shift lines down 
        if (is_full) {
            for (int j = i; j > 1; j--) {
                for (int k = 0; k < TETRIS_WIDTH; k++) {
                    grid[j][k] = grid[j-1][k];
                }
            }
            for (int j = 0; j < TETRIS_WIDTH; j++) {
                grid[0][j] = 0;
            }
            i++;
            rows_cleared++;
        }
    }

    if (rows_cleared > 0) {
        audio_play_tone(1300, 30);
        // calculate variables
        lines += rows_cleared; 
        level = lines / 10; 
        drop_interval = BASE_DROP_MS - (level * 40);
        if (drop_interval < 80) drop_interval = 80;

        if (rows_cleared == 1) score += 100 * (level + 1); 
        else if (rows_cleared == 2) score += 300 * (level + 1);
        else if (rows_cleared == 3) score += 500 * (level + 1);
        else if (rows_cleared == 4) score += 800 * (level + 1);

        // redraw grid
        for (int i = 0; i < TETRIS_HEIGHT; i++) {
            for (int j = 0; j < TETRIS_WIDTH; j++) {
                if (grid[i][j] == 0) draw_cell(i, j, tetris_bg); 
                else draw_cell(i, j, grid[i][j]);
            }
        }
    }
}

static uint8_t collision (Piece p) {
    for (int i = 0; i < 4; i++) {
        int8_t row = tetrominoes[p.type][p.rot][i][0] + p.y; 
        int8_t col = tetrominoes[p.type][p.rot][i][1] + p.x;
        if (col < 0 || col >= TETRIS_WIDTH || row >= TETRIS_HEIGHT) return 1; 
        if (row >= 0 && grid[row][col] != 0) return 1; 
    }
    return 0;
}

void tetris_init () {
    // Reset variables 
    for (int i = 0; i < TETRIS_HEIGHT; i++) {
        for (int j = 0; j < TETRIS_WIDTH; j++) {
            grid[i][j] = 0;
        }
    }
    game_over = 0; 
    drop_interval = BASE_DROP_MS; 
    hold_type = -1; 
    hold_used = 0;
    score = 0; 
    level = 0;
    lines = 0; 

    // Spawn first piece 
    current.type = HAL_GetTick() % PIECES;
    current.rot = 0;
    current.x = (TETRIS_WIDTH-4)/2;
    current.y = 0;
    next_type = HAL_GetTick() * 3 % PIECES;

    gfx_clear_screen(tetris_bg);

    // Draw border around playfield
    lcd_fill_rect(FIELD_X - 2, FIELD_Y - 2, TETRIS_WIDTH * TETRIS_TILE + 4, 2, tetris_border);  // top
    lcd_fill_rect(FIELD_X - 2, FIELD_Y + TETRIS_HEIGHT * TETRIS_TILE, TETRIS_WIDTH * TETRIS_TILE + 4, 2, tetris_border);  // bottom
    lcd_fill_rect(FIELD_X - 2, FIELD_Y, 2, TETRIS_HEIGHT * TETRIS_TILE, tetris_border);  // left
    lcd_fill_rect(FIELD_X + TETRIS_WIDTH * TETRIS_TILE, FIELD_Y, 2, TETRIS_HEIGHT * TETRIS_TILE, tetris_border);  // right

    draw_sidebar();
}

void tetris_input() {
    uint8_t pressed = buttons_pressed(); 

    if (pressed & BTN_DOWN) {
        Piece tmp = current; 
        tmp.y = tmp.y+2; 

        draw_piece(current, tetris_bg);
        if (!collision(tmp)) current = tmp; 
        draw_piece(current, piece_colours[current.type]);
    } else if (pressed & BTN_LEFT) {
        Piece tmp = current; 
        tmp.x = tmp.x-1; 

        draw_piece(current, tetris_bg);
        if (!collision(tmp)) current = tmp; 
        draw_piece(current, piece_colours[current.type]);
    } else if (pressed & BTN_RIGHT) {
        Piece tmp = current; 
        tmp.x = tmp.x+1; 

        draw_piece(current, tetris_bg);
        if (!collision(tmp)) current = tmp; 
        draw_piece(current, piece_colours[current.type]);
    } else if (pressed & BTN_A) {
        Piece tmp = current; 
        tmp.rot = (tmp.rot+1)%4; 

        draw_piece(current, tetris_bg);
        if (!collision(tmp)) current = tmp; 
        draw_piece(current, piece_colours[current.type]);
    } else if ((pressed & BTN_B) && (hold_used == 0)) {
        if (hold_type == -1) {
            hold_type = current.type;
            draw_piece(current, tetris_bg);
            current.type = next_type;
            next_type = HAL_GetTick() % PIECES;
        } else {
            int8_t tmp = hold_type;
            hold_type = current.type; 
            draw_piece(current, tetris_bg);
            current.type = tmp; 
        }
        current.rot = 0;
        current.x = (TETRIS_WIDTH-4)/2;
        current.y = 0;
        hold_used = 1;
        draw_piece(current, piece_colours[current.type]);
        draw_sidebar();
    }
}

void tetris_update() {
    // Update current piece 
    draw_piece(current, tetris_bg);
    Piece tmp = current; 
    tmp.y = tmp.y+1; 

    if (collision(tmp)) {
        draw_piece(current, piece_colours[current.type]);
        lock_piece(current, piece_colours[current.type]);
        clear_lines();
        current.type = next_type; 
        current.rot = 0;
        current.x = (TETRIS_WIDTH-4)/2;
        current.y = 0;
        next_type = HAL_GetTick() % PIECES;
        hold_used = 0;
        draw_sidebar();
        if (collision(current)) {
            game_over = 1; 
            return;
        }
    } else current = tmp;
    
    draw_piece(current, piece_colours[current.type]);
}

void tetris_run () {
    // Clear stale button state
    HAL_Delay(200);
    buttons_update();
    buttons_update();

    tetris_init();

    uint32_t last_update_time = HAL_GetTick();
    while (!game_over) {
        buttons_update(); 
        tetris_input(); 
        if (HAL_GetTick() - last_update_time > drop_interval) {
            tetris_update(); 
            last_update_time = HAL_GetTick();
        }
    }

    gfx_clear_screen(tetris_bg);
    gfx_draw_string(10, 10, "GAME OVER ...", 0xFFFF, tetris_bg);
    char score_str[20];
    sprintf(score_str, "Score: %d", score);
    gfx_draw_string(10, 40, score_str, 0xFFFF, tetris_bg);
    if (score > scores_get(TETRIS)) {
        scores_set(TETRIS, score); 
        scores_save(); 
        gfx_draw_string(10, 70, "NEW HIGH SCORE!", 0xF800, tetris_bg);
    } else {
        sprintf(score_str, "Best score: %d", scores_get(TETRIS));
        gfx_draw_string(10, 70, score_str, 0xFFFF, tetris_bg);
    }

    // Wait for any buttons to be released, then wait for B press to exit
    HAL_Delay(500);
    while (1) {
        buttons_update();
        if (buttons_pressed() & BTN_B) break;
    }
}