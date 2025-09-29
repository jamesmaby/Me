#ifndef __ADC_H__
#define __ADC_H__


#include "main.h"

typedef enum {
    ADC_VAL1,
    ADC_VAL2,
    ADC_VSENSE,
    ADC_VREF,

    __ADC_NB
}adc_e;

void ADC1_Init();

#endif // __ADC_H__