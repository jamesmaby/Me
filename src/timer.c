#include "timer.h"
#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"


void Timer_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_Prescaler = 48 - 1;
    
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 0xFFFF;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM6, &tim);
    TIM_Cmd(TIM6, ENABLE);
}

void Timer_DelayUs(uint32_t us) {
	
    uint16_t start = TIM6->CNT;
    while ((uint16_t)(TIM6->CNT - start) < us);
}
