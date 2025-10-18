#include "spi.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_rcc.h"


void spi_Init(){
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0); // SCK
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0); // MISO
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0); // MOSI
    
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_Level_3;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);
    
    SPI_InitTypeDef spi;
    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_1Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // 8MHz/16=500kHz
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &spi);

    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI_Transfer(uint8_t data) {
    while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
    *(__IO uint8_t *)&SPI1->DR = data;
    while (!(SPI1->SR & SPI_I2S_FLAG_RXNE));
    return *(__IO uint8_t *)&SPI1->DR;
}

void SPI_ReadRegisters(uint8_t startAddr, uint8_t* buffer, uint8_t len) {
    CS_LOW;
    SPI_Transfer(startAddr | 0x80);   // adresse + bit lecture

    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = SPI_Transfer(0xFF);
    }

    CS_HIGH;
}

void SPI_WriteRegisters(uint8_t startAddr, uint8_t* buffer, uint8_t len) {
    CS_LOW;
    SPI_Transfer(startAddr & 0x7F);   // adresse + bit écriture

    for (uint8_t i = 0; i < len; i++) {
        SPI_Transfer(buffer[i]);
    }

    CS_HIGH;
}
