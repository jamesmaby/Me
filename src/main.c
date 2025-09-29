#include "main.h"
#include "system.h"
#include "usart.h"
#include "stm32f0xx.h"

#include "meDelay.h"
#include "meFifo.h"
#include "usart.h"
#include "meFd.h"
#include "meConsole.h"

conCtx_t conCtx;
meFd_t meFd;

const char* promptName = "STM32F0";


int main(){


    System_Init();

    meFd_Init(&meFd);

	console_Init(&conCtx, &meFd);

    while (1)
    {
        console_Polling(&conCtx);
    }
}