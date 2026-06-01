#include <stdbool.h>
#include "main.h"
#include "io.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_exti.h"
#include "stm32f0xx_syscfg.h"
#include "button.h"

typedef struct {
	GPIO_TypeDef* 	port;
	uint8_t			n;
	uint8_t			mode;
	uint8_t			type;
	uint8_t			pupd;
	uint8_t			af;
}iodesc_t;

const iodesc_t ios[] = {
		IODESC( PIN_LED0, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP ),

		// Console
		// IODESCAF( PIN_USART2RX, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_1 ),
		// IODESCAF( PIN_USART2TX, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_1 ),

		// ADC
		IODESC( PIN_ADC1, GPIO_Mode_AN, GPIO_OType_PP, GPIO_PuPd_NOPULL ),
		IODESC( PIN_ADC2, GPIO_Mode_AN, GPIO_OType_PP, GPIO_PuPd_NOPULL ),

		// SPI
		// IODESC( PIN_SPI_CS, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP ),

		//USER_BUTTON
		IODESC( PIN_USER_BUTTON, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP ),

		// MCO
#ifdef __USE_MCO__
		IODESCAF( PIN_MCO, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF_0 ),
#endif
	
		IODESC_END()
};

void IO_ApplyConf( const iodesc_t* pCnf ){
	const iodesc_t*		p;
	GPIO_InitTypeDef	gpio;

	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	for( p=pCnf ;p->port; ++p){
		gpio.GPIO_Pin = 1<<(p->n);
		gpio.GPIO_Mode = p->mode;
		gpio.GPIO_OType = p->type;
		gpio.GPIO_PuPd = p->pupd;
		GPIO_Init(p->port, &gpio);
		
		if( p->mode == GPIO_Mode_AF ){
			GPIO_PinAFConfig( p->port, p->n, p->af );
		}
	}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	EXTI_InitTypeDef exti;
	exti.EXTI_Line = EXTI_Line13;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	NVIC_SetPriority(EXTI4_15_IRQn, 1);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line13);
		
		btn_OnPushed();
    }
}

void MCO_Init( ){
#ifdef __USE_MCO__
	uint8_t enable = true;
#else 
	uint8_t enable = false;
#endif
	RCC_MCOConfig( enable ? RCC_MCOSource_SYSCLK : RCC_MCOSource_NoClock, RCC_MCOPrescaler_32 );
}

void IO_Init(){
	IO_ApplyConf(ios);
	MCO_Init( );
	
	MEPIN_CLR(PIN_LED0);
	// MEPIN_SET(PIN_SPI_CS);
}




