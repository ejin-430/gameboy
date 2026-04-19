#include "spi_dma.h"

extern SPI_HandleTypeDef hspi1;

static volatile uint8_t dma_done = 1;

void spi_dma_init () {
    dma_done = 1;
}

void spi_dma_transmit (const uint8_t *data, uint16_t len) {
    dma_done = 0;
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)data, len);
}

void spi_dma_receive (uint8_t *data, uint16_t len) {
    dma_done = 0;
    HAL_SPI_Receive_DMA(&hspi1, data, len);
}

void spi_dma_wait () {
    while (!dma_done) {}
}

void HAL_SPI_TxCpltCallback (SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) dma_done = 1;
}

void HAL_SPI_RxCpltCallback (SPI_HandleTypeDef *hspi){
    if (hspi->Instance == SPI1) dma_done = 1;
}
