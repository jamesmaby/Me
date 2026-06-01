#ifndef BUTTON_H
#define BUTTON_H


typedef struct {
    uint8_t mode;
    uint32_t pushed_rel;
    volatile uint32_t cnt;
    uint8_t ledMode;
}btn_ctx_t;

void btn_OnPushed();
void btn_IncCnt();

#endif // BUTTON_H