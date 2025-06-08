#ifndef ME_USART1_H
#define ME_USART1_H

#include "meDelay.h"
#include "stm32f0xx.h"
#include <stdbool.h>
#include "MeFiFo.h"



void UART_Init();
void SendString(const char * str);
void SendNb(uint16_t number);
void ReceiveString();
void USART1_putc(const char c);
bool USART1_getc(char * pc);


void USART2_IRQHandler();
void NVIC_Initialisation();








#endif 