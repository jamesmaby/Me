#include "WS2812B.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_misc.h"

#include "meConsole.h"
#include "io.h"
#include "timer.h"
#include "system.h"
#include "stm32f0xx_spi.h"

/* 
 * SPI2 @ 24MHz
*/

#define WS2812B_NUM_LEDS 12

#define BITS_PER_LED     24
#define BITS_PER_SYMBOL  3 // 3 bits per WS2812B bit
#define RESET_PULSE      50 //bits

#define SIZE (sizeof(uint8_t)<<3)
#define BUFFER_SIZE ((WS2812B_NUM_LEDS * BITS_PER_LED * BITS_PER_SYMBOL) / SIZE + RESET_PULSE)

/***
 * 3 bits per WS2812B bit
 * 0 bit = 100
 * 1 bit = 110
 * 
 * |   RGB      |      Bitstream      | PWM Buffer
 * ex: 0b10110  | 110|100|100|110|110 | 0b110100100110110
***/

volatile uint8_t ws2812b_done = 1;

uint8_t ws2812bBuffer[BUFFER_SIZE] = {0};
rgb_t leds[WS2812B_NUM_LEDS];

uint8_t symbols[] = {
    0b100, // WS2812B 0
    0b110, // WS2812B 1
};

/*************** */

#define WS2812B_TIMEOUT 50  //ms
#define STEPS 64  //nombre de pas pour le dégradé
#define RIPPLE_MAX_RADIUS 6   // pour 12 LEDs

rgb_t rgb[] = {
    [RED]     = {255,   0,   0},
    [ORANGE]  = {255, 128,   0},
    [YELLOW]  = {255, 255,   0},
    [GREEN]   = {  0, 255,   0},
    [CYAN]    = {  0, 255, 255},
    [BLUE]    = {  0,   0, 255},
    [PURPLE]  = {128,   0, 255},
    [MAGENTA] = {255,   0, 255},
};

uint8_t tail[] = { 100, 65, 40, 20, 5 };

ws2812b_ctx_t ctx = {
    .colorIndex = 0,
    .luminosity = 20,
    .step = 0,
    .ledMode = LED_MODE_OFF,
    .delay = {WS2812B_TIMEOUT},
    .shift = 0,
    .idx = 0,
    .tail = tail,
    .tailSize = sizeof(tail)/sizeof(tail[0]),
};


void WS2812B_Init() {

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // GPIO PB15 SPI2_MOSI
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_15;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_Level_3;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpio);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_0);

    // SPI2
    SPI_InitTypeDef spi;
    spi.SPI_Direction = SPI_Direction_1Line_Tx;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_1Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &spi);

    // DMA1-ch5
    DMA_InitTypeDef dma;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
    dma.DMA_MemoryBaseAddr = (uint32_t)ws2812bBuffer;
    dma.DMA_DIR = DMA_DIR_PeripheralDST;
    dma.DMA_BufferSize = BUFFER_SIZE;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Normal;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(WS2818B_DMA, &dma);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    
    NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

    DMA_ITConfig(WS2818B_DMA, DMA_IT_TC, ENABLE);
}

void DMA1_Ch4_7_DMA2_Ch3_5_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC5)) {
        DMA_ClearITPendingBit(DMA1_IT_TC5);
        
        SPI_Cmd(SPI2, DISABLE);
        DMA_Cmd(WS2818B_DMA, DISABLE);
        
        ws2812b_done = 1;
    }
}

void WS2812B_PrepareBuffer() {
    uint32_t bitIdx = 0;

    memset(ws2812bBuffer, 0, sizeof(ws2812bBuffer));

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        uint32_t color = ((uint32_t)leds[i].g << 16) | ((uint32_t)leds[i].r << 8) | leds[i].b;

        for (int bit = 23; bit >= 0; bit--) {
            uint8_t symbol = (color & (1 << bit)) ? symbols[1] : symbols[0];

            for (int sbit = 2; sbit >= 0; sbit--, bitIdx++) 
            {
                uint32_t byteIdx = bitIdx / SIZE;
                uint8_t bitPos = 7 - (bitIdx & (SIZE-1));
                ws2812bBuffer[byteIdx] |= ((symbol >> sbit) & 0x1) << bitPos; // Set bit
            }
        }
    }
}

void WS2812B_Refresh() {
    WS2812B_PrepareBuffer();

    WS2818B_DMA->CMAR  = (uint32_t)ws2812bBuffer;
    WS2818B_DMA->CNDTR = BUFFER_SIZE;
    DMA_Cmd(WS2818B_DMA, ENABLE);
    SPI_Cmd(SPI2, ENABLE);
    ws2812b_done = 0;
}

void WS2812B_SetPixel(int idx, uint8_t r, uint8_t g, uint8_t b, uint8_t luminosity) {
    if (idx >= WS2812B_NUM_LEDS) return;

    leds[idx].r = (uint8_t)(((uint16_t)r * luminosity)/100);
    leds[idx].g = (uint8_t)(((uint16_t)g * luminosity)/100);
    leds[idx].b = (uint8_t)(((uint16_t)b * luminosity)/100);
}

void WS2812B_SetAllPixels(uint8_t r, uint8_t g, uint8_t b, uint8_t luminosity) {
    for (int i = 0; i < WS2812B_NUM_LEDS; i++) 
        WS2812B_SetPixel(i, r, g, b, luminosity);
}

void WS2812B_SwitchMode(uint8_t mode) {
    if( mode >= LED_MODE_NB ) return;

    ws2812b_ctx_t* p = &ctx;
    p->ledMode = mode;
    p->colorIndex = 0;
    p->step = 0;
    p->delay.timeout = GiveTime();
}

uint8_t WS2812B_GetMode() {
    ws2812b_ctx_t* p = &ctx;
    return p->ledMode;
}

void WS2812B_Poll() {
    ws2812b_ctx_t* p = &ctx;

    if (!meDelay(&p->delay) || (SPI2->SR & SPI_SR_BSY)) return;

    switch (p->ledMode)
    {
    case LED_MODE_OFF:
        WS2812B_SetAllPixels(0, 0, 0, 0);
        WS2812B_Refresh();
        p->ledMode = LED_MODE_NOP;
        p->delay.timeout = GiveTime();

    case LED_MODE_NOP:
        break;

    case LED_MODE_SWAP:
        {
            rgb_t c = rgb[p->colorIndex];
            WS2812B_SetAllPixels(c.r, c.g, c.b, p->luminosity);
            WS2812B_Refresh();

            p->colorIndex = (p->colorIndex + 1) & (COLOR_NB - 1);

            meDelayInit(&p->delay, 1000);
        }
        break;

    case LED_MODE_COLOR_CYCLE:
    {
        uint8_t nextColorIndex = (p->colorIndex + 1) & (COLOR_NB - 1);

        rgb_t c1 = rgb[p->colorIndex];
        rgb_t c2 = rgb[nextColorIndex];
        
        uint8_t r = c1.r + (((int)c2.r - c1.r) * p->step) / STEPS;
        uint8_t g = c1.g + (((int)c2.g - c1.g) * p->step) / STEPS;
        uint8_t b = c1.b + (((int)c2.b - c1.b) * p->step) / STEPS;

        WS2812B_SetAllPixels(r, g, b, p->luminosity);
        WS2812B_Refresh();

        if (++p->step >= STEPS) {
            p->colorIndex = nextColorIndex;
            p->step = 0;
        }

        meDelayInit(&p->delay, WS2812B_TIMEOUT);
    }
        break;

    case LED_MODE_COLOR_SHIFT_L:
        {
            WS2812B_SetAllPixels(0, 0, 0, 0);

            rgb_t c = rgb[PURPLE];

            for (int i = 0; i < p->tailSize; i++) {
                uint8_t idx = p->shift - i;

                if ((int8_t)idx < 0)
                    idx += WS2812B_NUM_LEDS;

                WS2812B_SetPixel(idx, c.r, c.g, c.b, p->tail[i]);
            }

            WS2812B_Refresh();

            p->shift = (p->shift + 1) % WS2812B_NUM_LEDS;

            meDelayInit(&p->delay, 100);
        }
        break;

    case LED_MODE_COLOR_SHIFT_R:
        {
            WS2812B_SetAllPixels(0, 0, 0, 0);

            rgb_t c = rgb[YELLOW];

            for (int i = 0; i < p->tailSize; i++) {
                int idx = p->shift - i;

                if (idx < 0)
                    idx += WS2812B_NUM_LEDS;

                WS2812B_SetPixel(idx, c.r, c.g, c.b, p->tail[p->tailSize - i - 1]);
            }

            WS2812B_Refresh();

            p->shift--;
            if ((int8_t)p->shift < 0)
                p->shift += WS2812B_NUM_LEDS;

            meDelayInit(&p->delay, 100);
        }
        break;

    case LED_MODE_COLOR_RIPPLE:
        {
            WS2812B_SetAllPixels(0, 0, 0, 0);

            rgb_t c = rgb[p->colorIndex];

            int center = WS2812B_NUM_LEDS / 2;

            for (int d = 0; d <= p->idx; d++) {
                int left  = center - d;
                int right = center + d;

                if (left >= 0) {
                    uint8_t lum = (uint8_t)((100 * (RIPPLE_MAX_RADIUS - d) *
                        (RIPPLE_MAX_RADIUS - d)) /
                        (RIPPLE_MAX_RADIUS * RIPPLE_MAX_RADIUS));

                    WS2812B_SetPixel(left, c.r, c.g, c.b, lum);
                }

                if (right <= WS2812B_NUM_LEDS) {
                    uint8_t lum = (uint8_t)((100 * (RIPPLE_MAX_RADIUS - d) *
                        (RIPPLE_MAX_RADIUS - d)) /
                        (RIPPLE_MAX_RADIUS * RIPPLE_MAX_RADIUS));

                    WS2812B_SetPixel(right%WS2812B_NUM_LEDS, c.r, c.g, c.b, (d==RIPPLE_MAX_RADIUS) ? 1 : lum);
                }
            }

            WS2812B_Refresh();

            if (++p->idx > RIPPLE_MAX_RADIUS) {
                p->colorIndex = (p->colorIndex + 1) % COLOR_NB;
                p->idx = 0;
            }

            meDelayInit(&p->delay, 120);
        }
        break;

    case LED_MODE_COLOR_PING_PONG: 
        {
            WS2812B_SetAllPixels(0, 0, 0, 0);

            uint8_t center = WS2812B_NUM_LEDS/2;

            if (p->idx & 0b1){
                for (uint8_t i = 0; i < center; i++ ){
                    rgb_t c = rgb[i];
                    WS2812B_SetPixel((2*i) % WS2812B_NUM_LEDS, c.r, c.g, c.b, p->luminosity);
                }
            }
            else{
                for (uint8_t i = 0; i < center; i++ ){
                    rgb_t c = rgb[i];
                    WS2812B_SetPixel((2*i+1) % WS2812B_NUM_LEDS, c.r, c.g, c.b, p->luminosity);
                }
            }

            if (++p->idx >= WS2812B_NUM_LEDS)
                p->idx = 0;

            WS2812B_Refresh();
            meDelayInit(&p->delay, 200);
            
            break;
        }
    
    default:
        break;
    }
}
