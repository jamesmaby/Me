#include "GenLS.h"
#include "stm32f0xx.h"
#include "stm32f0xx_usart.h"
#include "io.h"
#include "stm32f0xx_rcc.h"

USART2_DECLARE_PA2_PA3(console, 115200, 128, 1024);