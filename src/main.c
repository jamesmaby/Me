#include "main.h"
#include "system.h"
#include "usart.h"
#include "stm32f0xx.h"

#include "meDelay.h"
#include "meFifo.h"
#include "usart.h"
#include "meFd.h"
#include "meConsole.h"


#define DEF_CONS(_name, _size) conCtx_t conCtx; \
        char buff_cons_##_name[_size];          \
        char buff_seq_##_name [6];              \
        conCtx.buff_seq = buff_seq_##_name;     \
        conCtx.buffer = buff_cons_##_name;      \
        static meDelay_t cons_delay_##_name;    \
        conCtx.delay_cons = &cons_delay_##_name;


int main(){

    System_Init();

    meFd_t meFd;
    meFd_Init(&meFd);

    DEF_CONS(meCons, 64);

	console_Init(&conCtx, &meFd);

    while (1)
    {

        console_Polling(&conCtx);
    }
}