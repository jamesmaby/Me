
#include "meFileDescriptor.h"
#include "test_printf.h"


uint16_t me_sd_Write(me_sd_t *me_sd, char *buff, uint16_t n){
    uint16_t i = 0;
    if (me_sd->ops->CanWrite(me_sd->ctx, n)){
        while (i < n){
            me_sd->ops->putc(me_sd->ctx,*(buff++));
            i++;
        }
    }
    return i;
}

uint16_t me_sd_Read(me_sd_t *me_sd, char *buff, uint16_t n){
    uint16_t i = 0;
    if (me_sd->ops->CanRead(me_sd->ctx, n)){
        while(i < n){
            *(buff+i) = me_sd->ops->getc(me_sd->ctx);
            i++;
        }
        return i;
    }else{
        return 0;
    }
}

uint16_t  me_sd_Print( me_sd_t *me_sd, char *buff) {
    uint16_t compt = 0;

    while (*buff){
        me_sd->ops->putc(me_sd->ctx,*buff);
        buff++;
        compt++;
    }

    return (compt);

}

void me_sd_Printf(me_sd_t *me_sd, const char *format, ...) {
    va_list va;

    va_start(va, format);

    tiny_format(me_sd->ops->putc, me_sd->ctx, format, va);
    
    va_end(va);
}






