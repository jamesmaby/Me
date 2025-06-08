
#include "meUSART1.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_misc.h"
#include "io.h"

#define BAUDRATE 115200

extern char	rx_tab[BUFFER_SIZE];  // Pour remplir le buffer de réception des données (sans interruption) 
uint8_t indice = 0;               // Indice du buffer de réception des données (sans interruption) 

uint32_t bday = 1000;  // Voir me_Delay.c
delay_t delay_Rx;  // Voir me_Delay.c

bool is_Transmiting = 0;


// void UART_Init(){

//     RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

//     GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
// 	GPIOA->MODER |= (GPIO_MODER_MODER2_1) | (GPIO_MODER_MODER3_1);

//     GPIOA->AFR[0] &= ~(0x0000FF00);
// 	GPIOA->AFR[0] |=  0x000001100;

//     RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

//     RCC->CFGR3 &= ~(RCC_CFGR3_USART2SW);

//     USART2->CR1 |= USART_CR1_OVER8;
//     USART2->BRR = SystemCoreClock/BAUDRATE * 2;

//     USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

//     USART2->CR1 |= USART_CR1_RXNEIE | USART_CR1_TXEIE;

//     USART2->CR1 |= USART_CR1_UE;

// }


void UART_Init(void)
{
    USART_InitTypeDef uart;
    NVIC_InitTypeDef nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    uart.USART_BaudRate = BAUDRATE;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &uart);

    USART2->CR1 |= USART_CR1_RXNEIE | USART_CR1_TXEIE;
    // USART_ITConfig(USART2, USART_IT_RXNE | USART_IT_TXE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    nvic.NVIC_IRQChannel = USART2_IRQn;
    nvic.NVIC_IRQChannelPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

}

void USART2_IRQHandler()
{
    if (USART2->ISR & USART_ISR_RXNE){          // Test de l'origine de l'interruption

        meFiFo_Write(&fifo_Rx,USART2->RDR);
        //USART1_putc(USART1->RDR);        
	}

    if (USART2->ISR & USART_ISR_TXE) {            // Vérifier si la transmission est prête (TXE)
        if ( !(meFiFo_Empty(&fifo_Tx) == 1) ) {      // Lire depuis fifo_Tx
            USART2->TDR = meFiFo_Read(&fifo_Tx);        // Envoyer le caractère suivant
        }else {
            is_Transmiting = 0;
            USART2->CR1 &= ~USART_CR1_TXEIE; 
        }

    }
    
}

void SendString(const char * str){
    while (*str){
        USART1_putc(*(str++));
    }
}

void USART1_putc(const char c){

    USART2->CR1 &= ~USART_CR1_TXEIE; 

    if (is_Transmiting){
        meFiFo_Write(&fifo_Tx, c);
    }else{
        USART2->TDR = c;
        is_Transmiting = 1;
    }    
    USART2->CR1 |= USART_CR1_TXEIE; 
}

bool USART1_getc(char * pc){

    if ( !(meFiFo_Empty(&fifo_Rx)) ){
        *pc = meFiFo_Read(&fifo_Rx);
        return true;
    }else {
        return false;
    }
}


void SendNb(uint16_t number){
    uint8_t ind = 0;
    char buffer[6];

    if (number == 0) {  
        USART1_putc('0');
        return;
    }

    while (number > 0) {
        buffer[ind++] = (number % 10) + '0';  // Extraire le dernier chiffre
        number /= 10;
    }

    while (ind > 0) {

        USART1_putc(buffer[--ind]);

    }
}

// void ReceiveString(){
//     if ( USART1->ISR & USART_ISR_RXNE ){
//         if (indice < sizeof(rx_tab)){
//             rx_tab[indice++] = USART1->RDR;
//         }
//         meDelayInit(&delay,bday);
//     }
    
// }

void ReceiveString(){
    if ( USART2->ISR & USART_ISR_RXNE ){
        if (indice < sizeof(rx_tab)){
            rx_tab[indice++] = USART2->RDR;
        }
        meDelayInit(&delay_Rx,bday);
    }
    
}

