#include "usart.h"
#include "stm32f0xx.h"
#include "stm32f0xx_usart.h"
#include "io.h"

#define BAUDRATE 115200

#define ME_USART USART2

#define FIFO_RX_LEN 64
#define FIFO_TX_LEN 1024

USART_InitTypeDef usart;
bool isTransmitting = 0;

ME_FIFO_INIT(fifoRx, FIFO_RX_LEN);
ME_FIFO_INIT(fifoTx, FIFO_TX_LEN);

void MeUSART_Init(){

    usart.USART_BaudRate = BAUDRATE;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(ME_USART, &usart);
    
    ME_USART->CR1 |= USART_CR1_RXNEIE;
    ME_USART->CR1 |= USART_CR1_TXEIE;
    // USART_ITConfig(USART2, USART_IT_RXNE | USART_IT_TXE, ENABLE);

    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn);

    USART_Cmd(ME_USART, ENABLE);
}

void USART2_IRQHandler()
{
    if (ME_USART->ISR & USART_ISR_RXNE){
        char c = (uint8_t)ME_USART->RDR;
        meFifo_Putc(&fifoRx, c);
    }
    
    if (ME_USART->ISR & USART_ISR_TXE){
        if (!meFifoIsEmpty(&fifoTx)){
            char c;
            meFifo_Readc(&fifoTx, &c);
            ME_USART->TDR = c;
        } else {
            isTransmitting = false;
            ME_USART->CR1 &= ~USART_CR1_TXEIE;  // plus rien à envoyer
        }
    }
}

void MeUSART_Putc(void *ctx, char c){
    meCtx_t *p = (meCtx_t*)ctx;

    ME_USART->CR1 &= ~USART_CR1_TXEIE;
    if (isTransmitting){
        meFifo_Putc(p->fifoTx, c);
    } else {
        ME_USART->TDR = c;
        isTransmitting = true;
    }
    ME_USART->CR1 |= USART_CR1_TXEIE;
}

bool MeUSART_Getc(void *ctx, char *c){
    meCtx_t *p = (meCtx_t*)ctx;
    return meFifo_Readc(p->fifoRx, c);
}

bool MeUSART_CanWrite(void *ctx, uint16_t n){
    meCtx_t *p = (meCtx_t*)ctx;
    return meFifoFree(p->fifoRx, n);
}

bool MeUSART_CanRead(void *ctx, uint16_t n){
    meCtx_t *p = (meCtx_t*)ctx;
    return meFifoCnt(p->fifoRx, n);
}

void MeUSART_SendString(void *ctx, char *c){
    while (*c)
        MeUSART_Putc(ctx, *c++);
}