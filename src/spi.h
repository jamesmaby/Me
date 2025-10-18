#ifndef __SPI_H__
#define __SPI_H__

#include "io.h"

#define CS_LOW  MEPIN_CLR(PIN_SPI_CS)
#define CS_HIGH MEPIN_SET(PIN_SPI_CS)

void spi_Init();
uint8_t SPI_Transfer(uint8_t data);
void SPI_ReadRegisters(uint8_t startAddr, uint8_t* buffer, uint8_t len);
void SPI_WriteRegisters(uint8_t startAddr, uint8_t* buffer, uint8_t len);


#endif // __SPI_H__