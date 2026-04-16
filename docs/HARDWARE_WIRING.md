# Hardware Wiring – STM32 Nucleo Game Console

This document defines the wiring for the handheld console prototype using an **STM32 Nucleo MB1136 (Nucleo-64)** board.

The system connects:

* ILI9341 SPI LCD
* microSD card (SPI)
* 6 game buttons (D-pad + A/B)
* passive buzzer (PWM audio)

All devices run at **3.3V logic**.

---

# 1. Power Distribution

The Nucleo board is powered via USB.
Its onboard regulator provides **3.3V**, which is used to power external components.

| Nucleo Pin | Connects To   |
| ---------- | ------------- |
| 3V3        | LCD VCC       |
| 5V         | SD Card VCC   |
| GND        | LCD GND       |
| GND        | SD Card GND   |
| GND        | Button ground |
| GND        | Buzzer        |

All devices must share the **same ground reference**.

---

# 2. SPI Bus (Shared)

Both the LCD and SD card share the **SPI1 bus**.

| Function | Nucleo Pin | MCU Pin |
| -------- | ---------- | ------- |
| SPI SCK  | D13        | PA5     |
| SPI MISO | D12        | PA6     |
| SPI MOSI | D11        | PA7     |

Connections:

| Device | Signal | Nucleo |
| ------ | ------ | ------ |
| LCD    | SCK    | D13    |
| LCD    | MOSI   | D11    |
| SD     | SCK    | D13    |
| SD     | MOSI   | D11    |
| SD     | MISO   | D12    |

The LCD normally does **not require MISO**.

---

# 3. Chip Select Pins

Each SPI device needs its own **chip select (CS)**.

| Device | Pin | MCU |
| ------ | --- | --- |
| LCD CS | D10 | PB6 |
| SD CS  | D4  | PB5 |

Only one CS should be **LOW at a time**.

---

# 4. LCD Control Pins (ILI9341)

Additional control signals required by the display.

| LCD Pin           | Nucleo Pin | MCU |
| ----------------- | ---------- | --- |
| DC (Data/Command) | D9         | PC7 |
| RESET             | D8         | PA9 |
| SCK               | D13        | PA5 |
| MOSI              | D11        | PA7 |
| CS                | D10        | PB6 |
| VCC               | 3V3        | —   |
| LED               | 3V3        | —   |
| GND               | GND        | —   |

---

# 5. microSD Card

SD card operates in **SPI mode**.

| SD Pin | Nucleo Pin | MCU |
| ------ | ---------- | --- |
| SCK    | D13        | PA5 |
| MOSI   | D11        | PA7 |
| MISO   | D12        | PA6 |
| CS     | D4         | PB5 |
| VCC    | 5v         | —   |
| GND    | GND        | —   |

---

# 6. Game Buttons (6 Button Layout)

The prototype uses **6 buttons**:

* Up
* Down
* Left
* Right
* A
* B

Each button connects **GPIO → button → GND**.

Internal **pull-up resistors** will be enabled in firmware.

Pressed = **LOW**

| Button | Nucleo Pin | MCU  |
| ------ | ---------- | ---- |
| Up     | D2         | PA10 |
| Down   | D3         | PB3  |
| Left   | D5         | PB4  |
| Right  | D6         | PB10 |
| A      | D7         | PA8  |
| B      | A0         | PA0  |

---

# 7. Optional Future Buttons

If needed later:

| Button | Nucleo Pin | MCU |
| ------ | ---------- | --- |
| Start  | A1         | PA1 |
| Select | A2         | PA4 |

These are not required for the MVP.

---

# 8. Passive Buzzer (Audio)

A **passive piezo buzzer** is used so tones can be generated using PWM.

| Buzzer   | Nucleo Pin | MCU |
| -------- | ---------- | --- |
| Buzzer + | D15        | PB8 |
| Buzzer - | GND        | —   |

The buzzer will be driven using a **timer PWM output**.

Example frequencies:

| Frequency | Effect    |
| --------- | --------- |
| 400 Hz    | low tone  |
| 800 Hz    | medium    |
| 1200 Hz   | high tone |

---

# 9. Final Pin Summary

## SPI

| Pin       | Function |
| --------- | -------- |
| D13 (PA5) | SPI SCK  |
| D12 (PA6) | SPI MISO |
| D11 (PA7) | SPI MOSI |

## Device Control

| Pin       | Function  |
| --------- | --------- |
| D10 (PB6) | LCD CS    |
| D4 (PB5)  | SD CS     |
| D9 (PC7)  | LCD DC    |
| D8 (PA9)  | LCD RESET |

## Buttons

| Pin | Function |
| --- | -------- |
| D2  | Up       |
| D3  | Down     |
| D5  | Left     |
| D6  | Right    |
| D7  | A        |
| A0  | B        |

## Audio

| Pin | Function   |
| --- | ---------- |
| D15 | Buzzer PWM |

---

# 10. Pins Reserved / Avoid

These pins should not be used for other peripherals.

| Pin         | Reason              |
| ----------- | ------------------- |
| D11/D12/D13 | SPI bus             |
| D10         | LCD CS              |
| D4          | SD CS               |
| D0/D1       | UART debugging      |
| PC13        | Onboard user button |

---

# 11. Wiring Notes

* Keep **SPI wires short** for signal reliability.
* Ensure **CS lines default HIGH** when idle.
* Verify all devices support **3.3V logic**.
* Passive buzzer can be driven **directly from GPIO** for small piezos.

---

End of hardware wiring specification.
