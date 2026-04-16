# STM32 Game Console – Project Context (v1)

## Project Overview

This project is a custom handheld game console built on an STM32 Nucleo board.  
It is a firmware-focused embedded systems project intended to demonstrate:

- Peripheral driver development
- SPI device integration
- Real-time game loop design
- Graphics rendering on TFT LCD
- SD card file system integration
- Timer-based audio generation

The system will run multiple 2D games including:
- Snake
- Tetris
- Top-down shooter

---

## Hardware Configuration (v1)

### MCU
- STM32 Nucleo board (F4 family preferred)
- USB powered
- Built-in ST-LINK debugger

### Display
- ILI9341 SPI TFT LCD
- Resolution: 240×320
- 3.3V logic
- Connected via shared SPI bus

### Storage
- MicroSD SPI module
- Shares SPI bus with LCD
- Separate Chip Select (CS) pin

### Input
- 6 tactile push buttons:
  - Up, Down, Left, Right
  - A, B
- Connected to GPIO
- Internal pull-ups enabled in firmware

### Audio
- Passive piezo buzzer
- Driven by timer PWM output

---

## System Architecture

### Shared SPI Bus
- SPI_SCK → LCD + SD
- SPI_MOSI → LCD + SD
- SPI_MISO → SD only
- Separate CS pins for LCD and SD

### Firmware Structure (Planned)

/drivers
  lcd/
  sd/
  buttons/
  audio/

/graphics
/games
/system

---

## v1 Functional Goals

1. LCD initialization and basic drawing
2. Button input handling (polling or interrupt-based)
3. PWM-based sound effects
4. SD card initialization and file reading
5. Game selection menu
6. Snake → Tetris → Shooter progression

---

## Constraints (v1)

- USB powered only
- Breadboard wiring
- No custom PCB
- SPI-based rendering (no full framebuffer initially)

---

## Development Environment

- STM32CubeIDE
- USB debugging via ST-LINK
- UART logging for debugging