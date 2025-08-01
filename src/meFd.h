#ifndef __ME_FD_H
#define __ME_FD_H

#include "stdint.h"
#include "stdbool.h"
#include "meFifo.h"

typedef struct
{
    void (*putc)(void *ctx, char c);
    bool (*getc)(void *ctx, char *c);
    bool (*canWrite)(void *ctx, uint16_t n);
    bool (*canRead)(void *ctx, uint16_t n);
}meOps_t;

typedef struct
{
    meFifo_t *fifoRx;
    meFifo_t *fifoTx;
}meCtx_t;


typedef struct
{
    meOps_t *ops;
    meCtx_t *ctx;
}meFd_t;


void meFd_Init(meFd_t *p);
uint16_t meFd_Write(meFd_t *p, char *buff, uint16_t n);
uint16_t meFd_Read(meFd_t *p, char *buff, uint16_t n);
uint16_t meFd_Print(meFd_t *p, char *buff);


#endif // __ME_FD_H
