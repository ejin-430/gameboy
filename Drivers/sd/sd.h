#ifndef SD_H
#define SD_H

#include "../../Core/Inc/main.h"

// SD_CS = D4 = PB5
#define SD_CS_PORT GPIOB
#define SD_CS_PIN GPIO_PIN_5

uint8_t sd_init(void);  // 0 = ok
                        // 1 = CMD0 failed
                        // 2 = CMD8 bad response
                        // 3 = ACMD41 timeout
                        // 4 = CMD58 failed
uint8_t sd_read_block(uint32_t lba, uint8_t * buffer);  // Logic Block Addressing
                                                        // 0 = ok
                                                        // 5 = CMD17 rejected
                                                        // 6 = data start token timeout
uint8_t sd_write_block(uint32_t lba, const uint8_t * buffer);
                        // 0 = ok
                        // 7 = CMD24 rejected
                        // 8 = data response rejected (CRC/write error)
void sel_sd(void);
void desel_sd(void);

#endif