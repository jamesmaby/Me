
#include "meDelay.h"

void meDelayInit(delay_t* delay, uint32_t bday){

    delay->myDelay = meTime + bday;
    
}

volatile uint8_t meDelay(delay_t* delay){
    if (delay->myDelay != -1){
        if (meTime >= (delay->myDelay) ){
            return 1;
        }else{
            return 0;
        }
    }
    return 0;
}

void meDelayDisable(delay_t* delay){
    delay->myDelay = -1;
}




