# STM32 Handheld Game Console

A handheld game console built on the STM32F401RE Nucleo board, featuring a color TFT display, six-button input, PWM audio, and SD card storage for high scores. Runs three built-in games: Snake, Tetris, and a top-down shooter.

## Hardware

- **MCU:** STM32F401RE Nucleo board (ARM Cortex-M4, 84 MHz)
- **Display:** ILI9341 240x320 TFT LCD (SPI, 16-bit RGB565 color)
- **Storage:** MicroSD breakout module with onboard voltage regulator
- **Audio:** Speaker or buzzer driven by PWM
- **Input:** 6 tactile push buttons (active-low with internal pull-ups)

## Wiring

All peripherals connect to the Nucleo board headers. The display and SD card share the SPI1 bus and are active-low selected by separate chip-select lines.

### SPI1 Bus (shared)

| Signal | Pin  | Nucleo Header |
|--------|------|---------------|
| SCK    | PA5  | D13           |
| MOSI   | PA7  | D11           |
| MISO   | PA6  | D12           |

### ILI9341 Display

| Signal | Pin  | Nucleo Header |
|--------|------|---------------|
| CS     | PB6  | D10           |
| DC     | PC7  | D9            |
| RST    | PA9  | D8            |
| VCC    | 3.3V | 3V3           |
| GND    | GND  | GND           |

Connect SCK, MOSI, and MISO to the shared SPI1 lines above.

### MicroSD Breakout

| Signal | Pin  | Nucleo Header |
|--------|------|---------------|
| CS     | PB5  | D4            |
| VCC    | 5V   | 5V            |
| GND    | GND  | GND           |

The SD breakout has an onboard regulator and level shifter -- it takes 5V on VCC and outputs 3.3V logic on the SPI lines. Connect SCK, MOSI, and MISO to the shared SPI1 lines above.

### Buttons

All buttons are active-low. Internal pull-ups are enabled in firmware, so each button only needs to connect between its pin and GND.

| Button | Pin  | Nucleo Header |
|--------|------|---------------|
| Up     | PA10 | D2            |
| Down   | PB3  | D3            |
| Left   | PB4  | D5            |
| Right  | PB10 | D6            |
| A      | PA8  | D7            |
| B      | PA0  | A0            |

### Audio

| Signal  | Pin  | Nucleo Header |
|---------|------|---------------|
| Speaker | PB8  | D15           |
| GND     | GND  | GND           |

Driven by TIM4 Channel 3 PWM output. Connect a small speaker or passive buzzer between PB8 and GND.

## Games

**Snake** -- Classic snake on a 30x40 tile grid. Eat food to grow, avoid hitting yourself. D-Pad controls direction.

**Tetris** -- All 7 tetrominoes, rotation, hold piece, next piece preview, line clearing, and level-based speed increases. D-Pad moves and rotates, A hard-drops, B holds.

**Top-Down Shooter** -- Move left and right to dodge enemies and fire upward. Enemies spawn from the top and shoot back. Difficulty increases as your score climbs.

## High Scores

High scores are saved to the SD card using FatFS. Each game stores its own best score, which persists across power cycles.

## Building

This is an STM32CubeIDE project. Open the repository as an existing STM32CubeIDE project, build, and flash to the Nucleo board over the on-board ST-Link.
