#include "main.h"
#include "system.h"
#include "usart.h"
#include "stm32f0xx.h"

#include "meDelay.h"
#include "meFifo.h"
#include "usart.h"
#include "meFd.h"


int main(){

    System_Init();

    meFd_t meFdTest;
    meFd_Init(&meFdTest);


    char test[20] = "bien ou quoi";

    meFd_Print(&meFdTest, test);


    while (1)
    {

    }
}