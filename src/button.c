#include "stm32f0xx.h"
#include "stm32f0xx_exti.h"
#include "meDelay.h"
#include "system.h"
#include "button.h"
#include "io.h"
#include "meConsole.h"
#include "WS2812B.h"

extern uint8_t ledMode;
#define TO_LONG_PRESS 2000 //ms
#define TO_SHORT_PRESS 20 //ms
btn_ctx_t btn_ctx = {0};

typedef enum {
    IDLE,
    PRESS_DETECTED,
    PRESS_WAITING,
    RELEASE_DETECTED,
    LONG_PRESS_DETECTED,
    LONG_PRESS_WAITING,
}btn_e;

void btn_OnPushed() {
    btn_ctx_t* p = &btn_ctx;
    bool isPress = MEPIN_GET(PIN_USER_BUTTON) == 0;

    switch (p->mode)
    {
    case IDLE:
        if (isPress)
            p->mode = PRESS_WAITING;
        break;

    case PRESS_WAITING:
        if (!isPress) {
            p->mode = IDLE;
            p->cnt = 0;
        }
        break;

    case PRESS_DETECTED:
        if (!isPress) {
            p->ledMode = WS2812B_GetMode();
            WS2812B_SwitchMode( ++p->ledMode % (LED_MODE_NB));
            printf("ledmode [%d]\r\n", p->ledMode);
            p->mode = IDLE;
            p->cnt = 0;
        }
        break;

    case LONG_PRESS_DETECTED:
        if (!isPress) {
            WS2812B_SwitchMode( LED_MODE_OFF );
            p->mode = IDLE;
            p->cnt = 0;
        }
        break;
    
    default:
        break;
    }
}

void btn_IncCnt(){
    btn_ctx_t* p = &btn_ctx;

    if( (p->mode == PRESS_WAITING) ){
        if (++p->cnt >= TO_SHORT_PRESS)
            p->mode = PRESS_DETECTED;
        
    }else if( p->mode == PRESS_DETECTED ){
        if (++p->cnt >= TO_LONG_PRESS) {
            p->mode = LONG_PRESS_DETECTED;
            WS2812B_SwitchMode( LED_MODE_COLOR_PING_PONG );
        }
    }
}