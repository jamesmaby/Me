#include "i2c.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_i2c.h"
#include "stm32f0xx_rcc.h"

GPIO_InitTypeDef  gpio;
I2C_InitTypeDef i2c;

void I2C1_Init(){

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    /* PB6 = SCL | PB7 = SDA */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_2);

    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_Level_3;
    gpio.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOA, &gpio);

    I2C_DeInit(I2C1);
    I2C_StructInit(&i2c);

    /* Paramétrage cible : 8 MHz, duty 50 %, prescaler 4 → 2 MHz, SCLH=SCLL=60 */
    i2c.I2C_Timing = ( (4-1) & I2C_TIMINGR_PRESC )
                    | (60    & I2C_TIMINGR_SCLH )
                    | (60    & I2C_TIMINGR_SCLL)
                    | (1     & I2C_TIMINGR_SDADEL)
                    | (1     & I2C_TIMINGR_SCLDEL);
    
    
    i2c.I2C_OwnAddress1 = 0x00; /* Adresse propre de l'esclave (si esclave) */
    i2c.I2C_Mode = I2C_Mode_I2C;  /* Mode I2C */
    i2c.I2C_Ack = I2C_Ack_Enable; /* 7 bits */
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &i2c);

    /* Active I2C1 */
    I2C_Cmd(I2C1, ENABLE);
}

void I2C1_WriteByte(uint8_t addr, uint8_t data)
{
    // Configure transfert: adresse 7 bits, 1 byte, write, start
    I2C_TransferHandling(I2C1, addr<<1, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
    while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET);

    I2C_SendData(I2C1, data);

    // Attendre STOP
    while(I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) == RESET);
    I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}