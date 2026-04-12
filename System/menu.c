#include "menu.h"

static int cur_item = 0;
static int tot_items = 3;
static char* menu_items[] = {"Snake", "Tetris", "Top/Down Shooter"};

static void menu_draw() {
    gfx_draw_string(10, 10, "SELECT GAME", 0xFFFF, 0x0000);
    for (int i = 0; i < tot_items; i++) {
        // Selected = red bg + white text
        if (i == cur_item) gfx_draw_string(10, 40 + 20 * i, menu_items[i], 0x0000, 0xF800);
        // Not selected = white bg + black text
        else gfx_draw_string(10, 40 + 20 * i, menu_items[i], 0xFFFF, 0x0000);
    }
}

void menu_init() {
    gfx_clear_screen(0x0000);
    menu_draw();
}

int menu_update(void) {
    uint8_t pressed = buttons_pressed();

    // A to select 
    if (pressed & BTN_A) {
        audio_play_tone(1000, 80);
        return cur_item;
    }

    // Up/down navigation 
    if (pressed & BTN_UP) {
        if (cur_item == 0) cur_item = tot_items-1;
        else cur_item--;
        menu_draw();
        audio_play_tone(600, 30);
    } else if (pressed & BTN_DOWN) {
        if (cur_item == tot_items-1) cur_item = 0;
        else cur_item++;
        menu_draw();
        audio_play_tone(600, 30);
    } 
    
    return -1;
}