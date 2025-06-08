#ifndef MEDELAY_H
#define MEDELAY_H

#include <system.h>

#define BUFFER_SIZE (50 - 2)

typedef struct 
{
    uint64_t myDelay;
    
} delay_t;


void meDelayInit(delay_t* delay, uint32_t bday);
uint8_t meDelay(delay_t* delay);
void meDelayDisable(delay_t* delay);



#endif