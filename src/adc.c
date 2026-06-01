#include "adc.h"
#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_rcc.h"

#include "meConsole.h"

#define VREF_INT_CAL (uint16_t*)0x1FFFF7BA

uint16_t adc_buffer[__ADC_NB];

ADC_InitTypeDef adc;
DMA_InitTypeDef dma;

void DMA_ADC1_Init(){
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);

    dma.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
    dma.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_BufferSize = __ADC_NB;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &dma);

    ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void ADC1_Init(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_DeInit(ADC1);
    ADC_GetCalibrationFactor(ADC1);

    ADC_VrefintCmd(ENABLE);
    ADC_TempSensorCmd(ENABLE);

    adc.ADC_Resolution = ADC_Resolution_12b;
    adc.ADC_ContinuousConvMode = ENABLE;
    adc.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    // adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_ScanDirection = ADC_ScanDirection_Upward;
    ADC_Init(ADC1, &adc);

    /* PC0 PC1 VREF VSENS */
    ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_239_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_239_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor, ADC_SampleTime_239_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint, ADC_SampleTime_239_5Cycles);

    ADC_OverrunModeCmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY) == RESET);
    
    DMA_ADC1_Init();

    ADC_StartOfConversion(ADC1);
}

cons_cmd_rc_t console_fn_Adc(conCtx_t *ctx);

COMMANDE(adc, "adc", console_fn_Adc, "Donne la valeur des ADC",
                        "     adc : affiche toutes les valeurs\r\n"
                        "     adc cal : affiche vcal\r\n" );


cons_cmd_rc_t console_fn_Adc(conCtx_t *ctx){
    int status = CON_RC_DONE;

    if ( ARGNB(0) )
    {
        // if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOSEQ)) 
        //     ADC_ClearFlag(ADC1, ADC_FLAG_EOSEQ);
        // while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOSEQ)){}

        printf("ADC1 PC0 : %d\r\n", adc_buffer[ADC_VAL1]);
        printf("ADC1 PC1 : %d\r\n", adc_buffer[ADC_VAL2]);
        printf("ADC1 VREF : %d\r\n", adc_buffer[ADC_VREF]);
        printf("ADC1 VSENSE : %d\r\n", adc_buffer[ADC_VSENSE]);
        printf("VCAL: %d\r\n", (uint16_t)*((uint16_t*)VREF_INT_CAL));

    } else if ( ARGNB(1) )
    {
        if ( ARGEQ(1, "cal") ) {
            printf("VCAL: %d\r\n", (uint16_t)*((uint16_t*)VREF_INT_CAL));

        } else {
            printf("argument inconnu\r\n");
            status = CON_RC_BAD_ARG;
        }
    } else 
    {
        printf("bad usage : \r\n%s\r\n", ctx->com_cmd_desc->usage);
        status = CON_RC_BAD_ARG;
    }

    console_Prompt(ctx);
    return status;
}


