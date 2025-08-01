#include "meFd.h"
#include "usart.h"


void meFd_Init(meFd_t *p){

    static meCtx_t ctx;

    ctx.fifoRx = &fifoRx;
    ctx.fifoTx = &fifoTx;

    static const meOps_t ops = {
        .putc = MeUSART_Putc,
        .getc = MeUSART_Getc,
        .canWrite = MeUSART_CanWrite,
        .canRead = MeUSART_CanRead
    };

    p->ctx = &ctx;
    p->ops = (void*)&ops;
}

uint16_t meFd_Write(meFd_t *p, char *buff, uint16_t n){
    uint16_t i = 0;
    if (p->ops->canWrite(p->ctx, n))
    {
        while (*buff){
            p->ops->putc(p->ctx, *buff++);
            i++;
        }
    }
    return i;
}

uint16_t meFd_Read(meFd_t *p, char *buff, uint16_t n){
    uint16_t i = 0;
    if (p->ops->canRead(p->ctx, n))
    {
        while (i < n){
            p->ops->getc(p->ctx, buff);
            buff++;
            i++;
        }
    }
    return i;
}

uint16_t meFd_Print(meFd_t *p, char *buff){
    uint16_t i = 0;

    while (*buff){
        p->ops->putc(p->ctx, *buff);
        buff++;
        i++;
    }

    return i;
}

