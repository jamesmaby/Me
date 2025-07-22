#include <stm32f0xx.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_gpio.h>
#include "stm32f0xx.h"
#include "stm32f0xx_flash.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_misc.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_iwdg.h"

#include "system.h"
#include "io.h"


volatile uint64_t meTime = 0;

void SystemClockConfig();

//-----------------------------------------------------------------------------------------
void System_Init(void) {

	SystemClockConfig();

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	IO_Init();
	
	SysTick_Config( SystemCoreClock / 1000 );
	
	meTime = 0;
}

//-----------------------------------------------------------------------------------------

uint16_t blink = 0;  

void SysTick_Handler(void) //1 kHz
{
	meTime++;

	if ( blink ){
		blink--;
	}else{
		blink = 500 - 1;
		MEPIN_SWAP(PIN_LED0);
	}
}

uint32_t GiveTime(){
	return meTime;
}
uint32_t GiveTimeSec(){
	return meTime / 1000;
}

void SystemClockConfig(){

	uint16_t timeout;

	// Set HSI 48MHz
	RCC->CR2 |= RCC_CR2_HSI48ON;
	timeout = HSI_STARTUP_TIMEOUT;
	while (!(RCC->CR2 & RCC_CR2_HSI48RDY) && timeout) timeout--;

	// Selection de PLL en entrée
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	timeout = 10000;
	while (!(RCC->CR2 & RCC_CFGR_SWS_PLL) && timeout) timeout--;

	// HSI48/prediv * PLL
	RCC->CFGR |= RCC_CFGR_PLLMUL4;

	// Selection de HSI48/prediv en clock d'entrée
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI48_PREDIV;

	// PLL ON
	RCC->CR |= RCC_CR_PLLON;
	timeout = 10000;
	while (!(RCC->CFGR & RCC_CR_PLLRDY) && timeout) timeout--;

	// HSI48/prediv
	RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV4;

	// HSI48/prediv * PLL / Hprediv (clk AHB)
	RCC->CFGR |= RCC_CFGR_HPRE_DIV8;

	// Division par 4 de clock en sortie de PLL (clk APB)
	RCC->CFGR |= RCC_CFGR_PPRE_DIV4;

	// 24 MHz < SYSCLK ≤ 48 MHz
	FLASH->ACR |= FLASH_ACR_LATENCY;

	SystemCoreClockUpdate();
}

