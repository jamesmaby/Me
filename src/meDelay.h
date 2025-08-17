#ifndef __ME_DELAY_H
#define __ME_DELAY_H

#include "stdint.h"




typedef struct 
{
    uint64_t timeout;
}meDelay_t;


void meDelayInit(meDelay_t *p, uint32_t delay);
volatile uint8_t meDelay(meDelay_t *p);
void meDelayDisable(meDelay_t *p);





#endif