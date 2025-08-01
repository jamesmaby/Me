#ifndef __USART_H
#define __USART_H

#include <stdbool.h>
#include "meFifo.h"
#include "meFd.h"

extern bool isTransmitting;

extern meFifo_t fifoRx;
extern meFifo_t fifoTx;


void MeUSART_Init();

/* OPS */
void MeUSART_Putc(void *ctx, char c);
bool MeUSART_Getc(void *ctx, char *c);
bool MeUSART_CanWrite(void *ctx, uint16_t n);
bool MeUSART_CanRead(void *ctx, uint16_t n);

void MeUSART_SendString(void *ctx, char *c);


#endif