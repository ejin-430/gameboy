#include "scores.h"
#include <stdio.h> 
#include <string.h> 
#include "../FATFS/App/fatfs.h"

static uint16_t high_scores[NUM_GAMES] = {0};
static const char * game_names[NUM_GAMES] = {"Snake", "Tetris", "Shooter"};

uint16_t scores_get (uint8_t game_id) {
    return high_scores[game_id];
}

void scores_set (uint8_t game_id, uint16_t val) {
    high_scores[game_id] = val; 
}

void scores_load () {
    f_mount(&USERFatFS, USERPath, 1);           // Force-mount SD filesystem 
    if (f_open(&USERFile, "scores.txt", FA_READ) != FR_OK) return; 

    // Read values from scores.txt
    char buf[128];
    UINT bytes_read;
    f_read(&USERFile, buf, sizeof(buf) - 1, &bytes_read);
    buf[bytes_read] = '\0';

    // Parse line by line 
    char * line = strtok(buf, "\n"); 
    for (int i = 0; i < NUM_GAMES; i++) {
        if (line == NULL) break;
        char * eq = strchr(line, '=');      // eq = pointer to '=' in the file 
        if (eq && strncmp(line, game_names[i], eq-line) == 0) high_scores[i] = atoi(eq+1); // Store the value after '='
        line = strtok(NULL, "\n");          // Proceed to next line 
    }

    f_close(&USERFile);
}

void scores_save () {
    if (f_open(&USERFile, "scores.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) return; 

    for (int i = 0; i < NUM_GAMES; i++) {
        char line[32]; 
        sprintf(line, "%s=%d\n", game_names[i], high_scores[i]);
        UINT bw; 
        f_write(&USERFile, line, strlen(line), &bw);
    }

    f_close(&USERFile);
}