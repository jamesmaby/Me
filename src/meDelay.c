#include "meDelay.h"
#include "system.h"



void meDelayInit(meDelay_t *p, uint32_t delay){
    p->timeout = meTime + delay;
}

volatile uint8_t meDelay(meDelay_t *p){
    return (meTime >= p->timeout);
}

void meDelayDisable(meDelay_t *p){
    p->timeout = 0;
}