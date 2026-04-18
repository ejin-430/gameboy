# STM32 Game Console – Development Timeline

## Phase 1 — LCD Driver
**Files:** `drivers/lcd/lcd.c`, `drivers/lcd/lcd.h`
**Status:** Done

The ILI9341 has a specific startup sequence — you send it ~15 initialization commands over SPI before it displays anything. Each command is sent by pulling DC low (command mode), asserting CS, sending the byte, then pulling DC high (data mode) for any parameters.

Functions to build in order:
1. `lcd_write_cmd()` / `lcd_write_data()` — raw SPI byte transfer
2. `lcd_init()` — the full init command sequence
3. `lcd_set_window()` — tells the display which pixel region to write to (column/row address set commands)
4. `lcd_fill_rect()` — flood fills a rectangle with one color
5. `lcd_draw_pixel()` — single pixel

Colors are 16-bit RGB565 format (5 bits red, 6 green, 5 blue).

**Milestone:** Screen fills solid red. Then green. Then blue. If all three work, SPI and wiring are confirmed.

---

## Phase 2 — Button Driver
**Files:** `drivers/buttons/buttons.c`, `drivers/buttons/buttons.h`
**Status:** Done

GPIO polling with software debounce. A raw button read just checks `HAL_GPIO_ReadPin()` — LOW means pressed (because of pull-ups). Debounce means you only register a press after the pin has been stable for ~20ms, to filter mechanical bounce.

Functions to build:
1. `buttons_read_raw()` — returns bitmask of current pin states
2. `buttons_update()` — called every game loop tick, compares current vs previous state
3. `buttons_pressed()` / `buttons_held()` — distinguishes a fresh press from a hold

**Milestone:** UART log prints "UP pressed" / "A held" when you press buttons.

---

## Phase 3 — Graphics Layer
**Files:** `graphics/graphics.c`, `graphics/graphics.h`
**Status:** Done

Builds on the LCD driver. Since there is no full framebuffer (SPI is too slow to push 240x320x2 bytes every frame), drawing goes directly to the display but writes are batched to minimize SPI transactions.

Functions to build:
1. `gfx_draw_char()` — renders a single ASCII character using a bitmap font (small fixed font like 5x7)
2. `gfx_draw_string()` — calls draw_char in a loop
3. `gfx_draw_sprite()` — draws a small bitmap (used for game tiles)
4. `gfx_clear_screen()` — fills entire display with background color

**Milestone:** "HELLO WORLD" appears on screen with a colored background.

---

## Phase 4 — Audio Driver
**Files:** `drivers/audio/audio.c`, `drivers/audio/audio.h`
**Status:** Ready to test

TIM4 is configured for PWM on PB8. To play a tone, calculate the timer period from the desired frequency, set the ARR (auto-reload register), set the pulse width to 50% duty cycle (ARR/2), and start the PWM.

APB1 timer clock is 84 MHz. Frequency formula: `ARR = (84,000,000 / frequency) - 1`

Functions to build:
1. `audio_play_tone(freq_hz, duration_ms)` — sets ARR, starts PWM, stops after duration
2. `audio_stop()` — stops PWM immediately

**Milestone:** Buzzer plays a recognizable beep on button press.

---

## Phase 5 — Game Menu
**Files:** `system/menu.c`, `system/menu.h`
**Status:** Done

A simple scrollable list: "Snake", "Tetris", "Shooter". Up/Down moves a cursor, A selects. Draws the list using the graphics layer, highlights the selected item in a different color.

**Milestone:** Menu appears on boot, navigation works, selecting a game launches it.

---

## Phase 6 — Snake
**Files:** `games/snake.c`, `games/snake.h`
**Status:** Done

Snake state: a circular buffer of segment positions, a direction, and a food position. The game loop runs on a timer — every N milliseconds the snake moves one tile.

Key concepts:
- Fixed-size tile grid (e.g. 30x40 tiles on 240x320 screen = 8px tiles)
- Only redraw what changed (erase tail, draw new head) to avoid flickering
- Collision detection (head hits wall or self)

**Milestone:** Playable Snake with score shown on screen.

---

## Phase 7 — SD Card Driver
**Files:** `drivers/sd/sd.c`, `drivers/sd/sd.h`
**Status:** Done

SD in SPI mode has a specific init handshake (send 74+ clock cycles with CS high, then CMD0 to reset, CMD8 to check voltage, ACMD41 to initialize). FatFS (included via STM32CubeIDE) sits on top of this.

This is the hardest peripheral to bring up — SD cards are picky about timing and the init sequence. Doing it after LCD/buttons means you can display diagnostic info on screen while debugging.

**Milestone:** Reads a file from SD card and prints its contents over UART.

---

## Phase 8 — Tetris
**Files:** `games/tetris.c`, `games/tetris.h`
**Status:** Done

More complex state than Snake: 7 tetromino shapes each with 4 rotation states, a 10x20 grid, line clear detection, and scoring. The SPI-only rendering constraint matters more here — only redraw changed cells.

**Milestone:** Playable Tetris with line clears and game over screen.

---

## Phase 9 — Top-down Shooter
**Files:** `games/shooter.c`, `games/shooter.h`
**Status:** [ ] Not started

Player position, bullet list, enemy list, collision between bullets and enemies. Requires a simple entity system and frame-rate-independent movement.

**Milestone:** Player can move, shoot, enemies spawn and move toward player, hits are detected.

---

## Phase 10 — High Score Persistence
**Files:** `system/scores.c`, `system/scores.h`
**Status:** [ ] Not started

Uses the Phase 7 SD card driver via FatFS to persist per-game high scores across power cycles. Without this, every reset wipes scores from RAM.

A simple text file (e.g. `scores.txt`) on the FAT32-formatted card holds one line per game: `snake=42`, `tetris=1200`, `shooter=85`. On boot the file is read once into memory; when a game ends with a new high score, the file is rewritten.

Functions to build:
1. `scores_load()` — called once on boot. Mounts SD, opens `scores.txt`, parses each line, populates an in-memory struct. Returns gracefully if file missing (treat all scores as 0).
2. `scores_save()` — called when a game posts a new high score. Rewrites the whole file (small enough that no append/seek logic is needed).
3. `scores_get(game_id)` / `scores_set(game_id, value)` — accessors.

Each game (Snake, Tetris, Shooter) calls `scores_get` at start to display the high score, and `scores_set` + `scores_save` at game-over if beaten.

**Milestone:** Set a high score in Snake, power-cycle the board, see the score persist on the menu screen.

---

## Summary

| Phase | Topic | Key Concepts Learned | Status |
|-------|-------|----------------------|--------|
| 1 | LCD Driver | SPI protocol, display command sequences, RGB565 color | [ ] |
| 2 | Button Driver | GPIO polling, debounce, state machines | [ ] |
| 3 | Graphics Layer | Bitmap fonts, sprite rendering, draw optimization | [ ] |
| 4 | Audio Driver | Timer PWM, frequency math | [ ] |
| 5 | Game Menu | UI state machines | [ ] |
| 6 | Snake | Game loops, grid-based movement, collision | [ ] |
| 7 | SD Card Driver | SPI peripherals, FAT filesystem, FatFS | [ ] |
| 8 | Tetris | Rotation state machines, complex grid logic | [ ] |
| 9 | Top-down Shooter | Entity lists, frame-independent movement | [ ] |
| 10 | High Score Persistence | FatFS file I/O, simple text parsing, cross-game state | [ ] |
