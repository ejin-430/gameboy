#include "sd.h"
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;

// Set by sd_init() from CMD58's OCR (CCS bit).
// 1 = SDHC/SDXC (block-addressed), 0 = SDSC (byte-addressed).
// Needed by read/write to decide whether to multiply lba by 512.
static uint8_t sd_is_sdhc = 0;

static uint8_t spi_tx_rx_byte (uint8_t tx) {
uint8_t rx;
HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, HAL_MAX_DELAY);
return rx;
}

void sel_sd () {
HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, 0);
}

void desel_sd () {
HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, 1);
}

uint8_t sd_init () {
// WAKE UP
hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // Slow clock for init
HAL_SPI_Init(&hspi1); // Reinit SPI
desel_sd();
for (int i = 0; i < 10; i++) spi_tx_rx_byte (0xFF);
HAL_Delay(10);

// SEND CMD0 (GO_IDLE_STATE)
sel_sd();
spi_tx_rx_byte(0x40); // CMD0 opcode
spi_tx_rx_byte(0x00); // arg byte 3
spi_tx_rx_byte(0x00); // arg byte 2
spi_tx_rx_byte(0x00); // arg byte 1
spi_tx_rx_byte(0x00); // arg byte 0
spi_tx_rx_byte(0x95); // CRC7 + end bit
// Poll for r1 response
uint8_t r1;
for (int i = 0; i < 10; i++) {
r1 = spi_tx_rx_byte(0xFF);
if ((r1 & 0x80) == 0) break; // got a response!
}
desel_sd();
spi_tx_rx_byte(0xFF); // idle cycle
if (r1 != 0x01) return 1;

// SEND CMD8 (SEND_IF_COND)
uint8_t is_v2 = 0;
sel_sd();
spi_tx_rx_byte(0x48); // CMD8 opcode
spi_tx_rx_byte(0x00); // arg byte 3
spi_tx_rx_byte(0x00); // arg byte 2
spi_tx_rx_byte(0x01); // arg byte 1 (volt range 2.7-3.6V)
spi_tx_rx_byte(0xAA); // arg byte 0
spi_tx_rx_byte(0x87); // CRC
// Poll for r1 response
r1 = 0xFF;
for (int i = 0; i < 10; i++) {
r1 = spi_tx_rx_byte(0xFF);
if ((r1 & 0x80) == 0) break; // got a response!
}
if (r1 == 0x01) {
spi_tx_rx_byte(0xFF);
spi_tx_rx_byte(0xFF);
uint8_t voltage = spi_tx_rx_byte(0xFF); // read voltage
uint8_t pattern = spi_tx_rx_byte(0xFF); // read pattern (echoed)
if (voltage != 0x01 || pattern != 0xAA) {
desel_sd();
spi_tx_rx_byte(0xFF);
return 2;
}
is_v2 = 1;
} else if (r1 != 0x05) {
desel_sd();
spi_tx_rx_byte(0xFF);
return 2;
}
desel_sd();
spi_tx_rx_byte(0xFF);

// SEND ACMD41(SD_SEND_OP_COND)
r1 = 0xFF;
for (int i = 0; i < 1000; i++) {
// CMD55 (APP_CMD)
sel_sd();
spi_tx_rx_byte(0x77); // 0x40 | 55
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0xFF); // CRC don't-care
uint8_t cmd55_r1 = 0xFF;
for (int j = 0; j < 10; j++) {
cmd55_r1 = spi_tx_rx_byte(0xFF);
if ((cmd55_r1 & 0x80) == 0) break;
}
desel_sd();
spi_tx_rx_byte(0xFF);

// ACMD41
sel_sd();
spi_tx_rx_byte(0x69); // 0x40 | 41
spi_tx_rx_byte(is_v2 ? 0x40 : 0x00); // HCS bit if v2
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0xFF);
r1 = 0xFF;
for (int j = 0; j < 10; j++) {
r1 = spi_tx_rx_byte(0xFF);
if ((r1 & 0x80) == 0) break;
}
desel_sd();
spi_tx_rx_byte(0xFF);

if (r1 == 0x00) break; // card ready
HAL_Delay(1);
}
if (r1 != 0x00) return 3; // init timed out

// SEND CMD58 (READ_OCR)
sel_sd();
spi_tx_rx_byte(0x7A); // 0x40 | 58
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0x00);
spi_tx_rx_byte(0xFF);
r1 = 0xFF;
for (int i = 0; i < 10; i++) {
r1 = spi_tx_rx_byte(0xFF);
if ((r1 & 0x80) == 0) break;
}
if (r1 != 0x00) {
desel_sd();
spi_tx_rx_byte(0xFF);
return 4;
}
uint8_t ocr0 = spi_tx_rx_byte(0xFF);
spi_tx_rx_byte(0xFF); // ocr1 (unused)
spi_tx_rx_byte(0xFF); // ocr2 (unused)
spi_tx_rx_byte(0xFF); // ocr3 (unused)
desel_sd();
spi_tx_rx_byte(0xFF);
sd_is_sdhc = (ocr0 & 0x40) ? 1 : 0;

// Restore SPI to operational speed (~5.25 MHz, safe for jumper wires)
hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
HAL_SPI_Init(&hspi1);

return 0;
}

uint8_t sd_read_block (uint32_t lba, uint8_t * buffer) {
// SDHC addresses by block number, SDSC by byte offset.
uint32_t addr = sd_is_sdhc ? lba : lba * 512;

// CMD17 (READ_SINGLE_BLOCK)
sel_sd();
spi_tx_rx_byte(0x51); // 0x40 | 17
spi_tx_rx_byte((addr >> 24) & 0xFF);
spi_tx_rx_byte((addr >> 16) & 0xFF);
spi_tx_rx_byte((addr >> 8) & 0xFF);
spi_tx_rx_byte((addr >> 0) & 0xFF);
spi_tx_rx_byte(0xFF); // CRC don't-care

// Poll R1
uint8_t r1 = 0xFF;
for (int i = 0; i < 10; i++) {
r1 = spi_tx_rx_byte(0xFF);
if ((r1 & 0x80) == 0) break;
}
if (r1 != 0x00) {
desel_sd();
spi_tx_rx_byte(0xFF);
return 5; // CMD17 rejected
}

// Wait for the data start token (0xFE). Card sends 0xFF while busy.
// Any other byte with bit 7 clear is an error token.
uint8_t token = 0xFF;
for (int i = 0; i < 20000; i++) { // ~generous timeout
token = spi_tx_rx_byte(0xFF);
if (token != 0xFF) break;
}
if (token != 0xFE) {
desel_sd();
spi_tx_rx_byte(0xFF);
return 6; // no/bad data token
}

// Read 512 data bytes in one HAL call (faster than byte-by-byte).
// HAL_SPI_Receive transmits 0x00 dummies, which the card ignores here.
HAL_SPI_Receive(&hspi1, buffer, 512, HAL_MAX_DELAY);

// Discard 2 CRC bytes (CRC is off in SPI mode — card still sends them).
spi_tx_rx_byte(0xFF);
spi_tx_rx_byte(0xFF);

desel_sd();
spi_tx_rx_byte(0xFF); // idle cycle
return 0;
}

uint8_t sd_write_block (uint32_t lba, const uint8_t * buffer) {
uint32_t addr = sd_is_sdhc ? lba : lba * 512;

// CMD24 (WRITE_SINGLE_BLOCK)
sel_sd();
spi_tx_rx_byte(0x58); // 0x40 | 24
spi_tx_rx_byte((addr >> 24) & 0xFF);
spi_tx_rx_byte((addr >> 16) & 0xFF);
spi_tx_rx_byte((addr >> 8) & 0xFF);
spi_tx_rx_byte((addr >> 0) & 0xFF);
spi_tx_rx_byte(0xFF);

// Poll R1
uint8_t r1 = 0xFF;
for (int i = 0; i < 10; i++) {
r1 = spi_tx_rx_byte(0xFF);
if ((r1 & 0x80) == 0) break;
}
if (r1 != 0x00) {
desel_sd();
spi_tx_rx_byte(0xFF);
return 7; // CMD24 rejected
}

// One dummy byte between command response and data packet (spec-required gap).
spi_tx_rx_byte(0xFF);

// Data packet: start token 0xFE, 512 bytes, 2 CRC bytes (ignored).
spi_tx_rx_byte(0xFE);
HAL_SPI_Transmit(&hspi1, (uint8_t *)buffer, 512, HAL_MAX_DELAY);
spi_tx_rx_byte(0xFF); // CRC byte 1 (don't care)
spi_tx_rx_byte(0xFF); // CRC byte 2 (don't care)

// Data response token: low 5 bits encode the result.
// xxx00101 (0x05) = accepted
// xxx01011 (0x0B) = CRC error
// xxx01101 (0x0D) = write error
uint8_t resp = spi_tx_rx_byte(0xFF);
if ((resp & 0x1F) != 0x05) {
desel_sd();
spi_tx_rx_byte(0xFF);
return 8; // data rejected
}

// Wait while card is writing. Card holds MISO LOW (0x00) while busy;
// reads as non-zero once done. Can take tens of ms for flash program.
for (int i = 0; i < 100000; i++) {
if (spi_tx_rx_byte(0xFF) != 0x00) break;
}

desel_sd();
spi_tx_rx_byte(0xFF);
return 0;
}
