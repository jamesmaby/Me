#include "main.h"
#include "system.h"
#include "usart.h"
#include "stm32f0xx.h"
#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_rtc.h"

#include "meDelay.h"
#include "meFifo.h"
#include "usart.h"
#include "meFd.h"
#include "meConsole.h"
#include "wd.h"
#include "rtc.h"

conCtx_t conCtx;
meFd_t meFd;
static meDelay_t delay;

const char* promptName = "STM32F072";


int main(){


    System_Init();

    meFd_Init(&meFd);

	console_Init(&conCtx, &meFd);

    wd_CheckRstSrc();

    meDelayInit(&delay, 1000);

    while (1)
    {
        console_Polling(&conCtx);


#ifdef __WD_ENABLE__
        IWDG_ReloadCounter();
#endif

    }
}