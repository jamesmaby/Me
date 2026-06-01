#include "rtc.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_rtc.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_pwr.h"
#include "stm32f0xx_exti.h"

#include "meConsole.h"


RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
RTC_AlarmTypeDef RTC_AlarmStruct;

void rtc_Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE); // autoriser l’accès au domaine backup

    // activer LSI pour alimenter le RTC
    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    // sélectionner LSI comme source RTC
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    RCC_RTCCLKCmd(ENABLE);

    RTC_WaitForSynchro();

    RTC_InitTypeDef RTC_InitStruct;
    RTC_StructInit(&RTC_InitStruct);

    RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
    RTC_InitStruct.RTC_AsynchPrediv = 125; // valeur typique pour LSI 40kHz
    RTC_InitStruct.RTC_SynchPrediv = 319; // valeur typique pour LSI 40kHz


    if (RTC_Init(&RTC_InitStruct) == ERROR)
        while(1); // erreur init RTC

    // Définir l’heure actuelle
    RTC_TimeStruct.RTC_Hours = 12;
    RTC_TimeStruct.RTC_Minutes = 0;
    RTC_TimeStruct.RTC_Seconds = 0;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

    // Définir la date (facultatif)
    RTC_DateStruct.RTC_Year = 25;
    RTC_DateStruct.RTC_Month = RTC_Month_October;
    RTC_DateStruct.RTC_Date = 6;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
}

void rtc_Alarm_Init(void)
{
    // Configurer une alarme A à +5 secondes
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
    uint8_t next_second = (RTC_TimeStruct.RTC_Seconds + 5) % 60;
    // uint8_t next_minute = (RTC_TimeStruct.RTC_Minutes + 1) % 60;

    RTC_AlarmStructInit(&RTC_AlarmStruct);
    RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours   = RTC_TimeStruct.RTC_Hours;
    RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = RTC_TimeStruct.RTC_Minutes;
    RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = next_second;
    RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStruct.RTC_AlarmDateWeekDay = 0;
    RTC_AlarmStruct.RTC_AlarmMask =  RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Hours; 

    
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
    
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = EXTI_Line17;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

	NVIC_SetPriority(RTC_IRQn, 2);
	NVIC_EnableIRQ(RTC_IRQn);
}

// Routine d’interruption alarme
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_ALRA) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        EXTI_ClearITPendingBit(EXTI_Line17); 
        rtc_DisplayTime(); // affiche l’heure au moment exact de l’alarme
        rtc_Alarm_Init();
    }
}

void rtc_DisplayTime(void)
{
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
    printf("ALARME! Heure = %02d:%02d:%02d\r\n",
           RTC_TimeStruct.RTC_Hours,
           RTC_TimeStruct.RTC_Minutes,
           RTC_TimeStruct.RTC_Seconds);
}

