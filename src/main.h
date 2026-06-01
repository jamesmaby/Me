#ifndef __MAIN_H
#define __MAIN_H

#include "stdio.h"

extern struct CONCTX_T conCtx;

#define printf(...) meFd_Printf(conCtx.pme_sd, __VA_ARGS__)
extern const char* promptName;

#endif