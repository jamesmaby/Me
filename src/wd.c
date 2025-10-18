#include "wd.h"
#include "stm32f0xx_iwdg.h"
#include "meConsole.h"


void wd_Init(){
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload( (IWDG_TIMEOUT_MS * LSI_VALUE) / 256 ); // TOms = (RELOAD*PSCK/LSI_VALUE) 1s timeout = (256*x/40000)
    IWDG_ReloadCounter();
    IWDG_Enable();
}

void wd_CheckRstSrc() {
    uint32_t flags = RCC->CSR;

    if (flags & RCC_CSR_IWDGRSTF) {
        printf("Redémarrage causé par le IWDG (Watchdog interne)\r\n");
    }
    else if (flags & RCC_CSR_WWDGRSTF) {
        printf("Redémarrage causé par le WWDG (Watchdog fenêtre)\r\n");
    }
    else if (flags & RCC_CSR_PINRSTF) {
        printf("Reset externe (bouton reset ou pin NRST)\r\n");
    }
    else if (flags & RCC_CSR_PORRSTF) {
        printf("Reset d'alimentation (Power-On Reset)\r\n");
    }
    else if (flags & RCC_CSR_SFTRSTF) {
        printf("Reset logiciel (via NVIC_SystemReset ou autre)\r\n");
    }
    else if (flags & RCC_CSR_LPWRRSTF) {
        printf("Reset causé par une sortie de mode veille (Wakeup from Standby)\r\n");
    }
    else {
        printf("Source de reset inconnue\r\n");
    }

    RCC->CSR |= RCC_CSR_RMVF;
}