

#include "meUSARTFD.h"

extern bool is_Transmiting;

void fd_FIFO_USART1_Init(me_sd_t *me_sd, MeFiFo_t *fiforx,  MeFiFo_t *fifotx) {

    static USART1_ctx_t USART1_ctx;

    USART1_ctx.fifoRx = fiforx;
    USART1_ctx.fifoTx = fifotx;

    static const me_sd_ops_t ops = {
        .putc = USART1_putc_fd,
        .getc = USART1_getc_fd,
        .CanRead = USART1_CanRead,
        .CanWrite = USART1_CanWrite
    };

    me_sd->ops = &ops;
    me_sd->ctx = &USART1_ctx;
}


void USART1_putc_fd(void *ctx, const char c){

    USART1_ctx_t *USART1_ctx = (USART1_ctx_t *)ctx;

    USART2->CR1 &= ~USART_CR1_TXEIE; 

    if (is_Transmiting){
        meFiFo_Write(USART1_ctx->fifoTx, c);
    }else{
        USART2->TDR = c;
        is_Transmiting = 1;
    }    
    USART2->CR1 |= USART_CR1_TXEIE; 
}

char USART1_getc_fd(void *ctx){
    USART1_ctx_t *USART1_ctx = (USART1_ctx_t *)ctx;

    return  meFiFo_Read(USART1_ctx->fifoRx);   // meFiFo_Read() gère déjà le cas où le buffer est vide

}

bool USART1_CanRead(void *ctx, uint16_t n){
    USART1_ctx_t *USART1_ctx = (USART1_ctx_t*)ctx;

    uint16_t cnt = meFiFo_Count(USART1_ctx->fifoRx);

    return (cnt >= n);
}

bool USART1_CanWrite(void *ctx, uint16_t n){
    USART1_ctx_t *USART1_ctx = (USART1_ctx_t*)ctx;

    uint16_t cnt = meFiFo_Free(USART1_ctx->fifoRx);

    return (cnt >= n);
}
