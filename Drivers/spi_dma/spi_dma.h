#ifndef SPI_DMA_H
#define SPI_DMA_H

#include "../../Core/Inc/main.h"

void spi_dma_init(void);
void spi_dma_transmit(const uint8_t *data, uint16_t len);
void spi_dma_receive(uint8_t *data, uint16_t len);
void spi_dma_wait(void);

#endif
