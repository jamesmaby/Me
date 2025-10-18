#ifndef __WD_H__
#define __WD_H__

#define IWDG_TIMEOUT_MS (2) // Durée avant reset du watchdog en s


void wd_Init();
void wd_CheckRstSrc();




#endif // __WD_H__