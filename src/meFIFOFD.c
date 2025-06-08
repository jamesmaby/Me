
#include "meFIFOFD.h"


void fd_FIFO_Init(me_sd_t *me_sd, uint16_t size) {
    static MeFiFo_t contexte;
    char buffer[size];

    contexte.buff = buffer;
    contexte.mask = size - 1;
    contexte.rd = 0;
    contexte.wr = 0;

    static const me_sd_ops_t ops = {
        .putc = meFiFo_Write,
        .getc = meFiFo_Read,
        .CanRead = meFiFo_CanRead,
        .CanWrite = meFiFo_CanWrite
    };

    me_sd->ops = &ops;
    me_sd->ctx = &contexte;
}