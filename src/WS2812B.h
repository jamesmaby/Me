#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include "meDelay.h"

extern volatile uint8_t ws2812b_done;

#define WS2818B_DMA DMA1_Channel5

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed)) rgb_t;

typedef struct {
    uint8_t colorIndex; // Choose which color
    uint8_t luminosity;
    
    uint8_t shift; // Shift from an original position
    uint8_t idx; // Led position
    uint8_t step; // For linear interpolation

    const uint8_t* tail; // For LED_MODE_COLOR_CYCLE mode
    uint8_t tailSize; // For LED_MODE_COLOR_CYCLE mode
    
    uint8_t ledMode;
    
    meDelay_t delay;
} ws2812b_ctx_t;

typedef enum {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    PURPLE,
    MAGENTA,

    COLOR_NB,
} color_t;

typedef enum {
    LED_MODE_OFF,
    LED_MODE_NOP,
    LED_MODE_SWAP,
    LED_MODE_COLOR_CYCLE,
    LED_MODE_COLOR_SHIFT_L,
    LED_MODE_COLOR_SHIFT_R,
    LED_MODE_COLOR_RIPPLE,
    LED_MODE_COLOR_PING_PONG,

    LED_MODE_NB,
} ws2812b_mode_t;

void WS2812B_SetAllPixels(uint8_t r, uint8_t g, uint8_t b, uint8_t luminosity);
void WS2812B_SetPixel(int idx, uint8_t r, uint8_t g, uint8_t b, uint8_t luminosity);
void WS2812B_Refresh();
void WS2812B_Init();

void WS2812B_Poll();
void WS2812B_SwitchMode(uint8_t mode);
uint8_t WS2812B_GetMode();

#endif // WS2812B_H