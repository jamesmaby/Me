#ifndef MEFILEDESCRIPTOR
#define MEFILEDESCRIPTOR

#include "stdbool.h"
#include "MeFiFo.h"


typedef struct {
    void (*putc)(void *pctx, char c);
    char (*getc)(void *pctx);
    bool (*CanRead)(void *pctx,uint16_t n);
    bool (*CanWrite)(void *pctx,uint16_t n);
}me_sd_ops_t;

typedef struct {
    MeFiFo_t *fifoRx;
    MeFiFo_t *fifoTx;
}USART1_ctx_t;

typedef struct {
    const me_sd_ops_t *ops;
    void *ctx;
}me_sd_t;


uint16_t me_sd_Read(me_sd_t *me_sd, char *buff, uint16_t n);
uint16_t me_sd_Print( me_sd_t *me_sd, char *buff);
uint16_t me_sd_Write(me_sd_t *me_sd, char *buff, uint16_t n);
void me_sd_Printf(  me_sd_t *me_sd, const char *format, ...);

#endif