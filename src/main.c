#include "main.h"
#include "system.h"
#include "stm32f0xx.h"
#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_rtc.h"

#include "meConsole.h"
#include "wd.h"
#include "rtc.h"
#include "WS2812B.h"

conCtx_t conCtx;

const char* promptName = "STM32F072";

int main(){

    System_Init();

	console_Init(&conCtx);

    wd_CheckRstSrc();

    while (1)
    {
        console_Polling(&conCtx);

        WS2812B_Poll();

#ifdef __WD_ENABLE__
        IWDG_ReloadCounter();
#endif

    }
}

void HardFault_Handler(void){
    __ASM("BKPT #0");
}