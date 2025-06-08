#ifndef MEFIFO_H
#define MEFIFO_H

#include <stdbool.h>
#include "stm32f0xx.h"

#define FIFO_RX_SIZE 64
#define FIFO_TX_SIZE 1024

typedef struct 
{
    uint16_t rd,wr; // pointeur de lecture du buffer (emplacement de lecture et d'écriture)
    uint16_t mask;  // taille de N octets (16 par exemple)
    char * buff; // Le buffer
}MeFiFo_t;

extern MeFiFo_t fifo_Rx;
extern char fifo_Rx_buffer[FIFO_RX_SIZE];  

extern MeFiFo_t fifo_Tx;
extern char fifo_Tx_buffer[FIFO_TX_SIZE];  

#define DEFINE_FIFO(_name,_size)        \
\
char _name ## _buffer[_size];           \
\
MeFiFo_t _name = {                      \
	.rd = 0,                            \
	.wr = 0,                            \
    .mask = _size-1,                    \
    .buff = _name ## _buffer            \
};


void meFiFo_Init(void * fifo);
void meFiFo_Write(void * pfifo, char element);
char meFiFo_Read(void * pfifo);
bool meFiFo_Empty(void * fifo);
bool meFiFo_Full(void * fifo);
bool meFiFo_CanWrite(void * fifo);
bool meFiFo_CanRead(void * fifo);
uint16_t meFiFo_Count(void * fifo);
uint16_t meFiFo_Free(void * fifo);


#endif