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
#include "GenLS.h"
#include "adc.h"
#include "wd.h"
#include "rtc.h"
#include "timer.h"
#include "WS2812B.h"
#include "button.h"


// #define __USE_MCO__
// #define __WD_ENABLE__
// #define __RTC_ALARM_ENABLE__

volatile uint64_t meTime = 0;

void SystemClockConfig();

//-----------------------------------------------------------------------------------------
void usart_init_console(); // delared in usart.c
void System_Init(void) {

	SystemClockConfig();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
		
	Timer_Init();
	ADC1_Init();
	
	IO_Init();
	usart_init_console();
	WS2812B_Init();

	rtc_Init();

#ifdef __WD_ENABLE__
	wd_Init();
#endif
#ifdef __RTC_ALARM_ENABLE__
	rtc_Alarm_Init();
#endif

	SysTick_Config( SystemCoreClock / 1000 );
	
	meTime = 0;
}

//-----------------------------------------------------------------------------------------

uint16_t blink = 500;  

void SysTick_Handler(void) // 1 kHz
{
	meTime++;

	if ( ! --blink ){
		blink = 500 - 1;
		MEPIN_SWAP(PIN_LED0);
	}

	btn_IncCnt();
}

void SystemClockConfig(){

	uint16_t timeout;

	// Set HSI 48MHz
	RCC->CR2 |= RCC_CR2_HSI48ON;
	
	timeout = HSI_STARTUP_TIMEOUT;
	while (!(RCC->CR2 & RCC_CR2_HSI48RDY) && timeout) timeout--;

	// PLLCLK = HSI48/prediv * PLL
	RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV2; // HSI48 / 2 = 24 MHz
	RCC->CFGR |= RCC_CFGR_PLLMUL; // x2 => 48 MHz  

	// PLL ON
	RCC->CR |= RCC_CR_PLLON;
	timeout = 10000;
	while (!(RCC->CR & RCC_CR_PLLRDY) && timeout) timeout--;

	// Selection de HSI48/prediv en clock d'entrée
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI48_PREDIV;

	// AHB : HSI48/prediv * PLL / Hprediv (clk AHB)
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	// APB : HSI48/prediv * PLL / Pprediv*Hprediv (clk APB)
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;

	// 24 MHz < SYSCLK ≤ 48 MHz
	FLASH->ACR |= FLASH_ACR_LATENCY;

	// Selection de PLL en entrée
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	timeout = 10000;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL) && timeout) timeout--;

	SystemCoreClockUpdate();
}

