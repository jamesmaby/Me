#ifndef MEUSARTFD
#define MEUSARTFD


#include "meFileDescriptor.h"
#include "MeFiFo.h"
#include <stdbool.h>

void fd_FIFO_USART1_Init(me_sd_t *me_sd, MeFiFo_t *fiforx,  MeFiFo_t *fifotx);

void USART1_putc_fd(void *ctx, const char c);
char USART1_getc_fd(void *ctx);
bool USART1_CanRead(void *ctx, uint16_t n);
bool USART1_CanWrite(void *ctx, uint16_t n);










#endif