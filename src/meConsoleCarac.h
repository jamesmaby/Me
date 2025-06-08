#ifndef MECONSOLECARAC
#define MECONSOLECARAC

#include "meConsole.h"
#include <stdbool.h>
#include "test_printf.h"




void console_Clear(ctx_cons_t *ctx);
bool console_entrer(ctx_cons_t *ctx);
void console_Backspace(ctx_cons_t *ctx);
void console_LeftArrow(ctx_cons_t *ctx);
void console_RightArrow(ctx_cons_t *ctx);
void console_UpArrow(ctx_cons_t *ctx);
void console_Echap(ctx_cons_t *ctx);
void console_Carac (ctx_cons_t *ctx, char c);
void console_Home(ctx_cons_t *ctx);
void console_End(ctx_cons_t *ctx);
void console_Del(ctx_cons_t *ctx);

bool console_GetPreviousCommand(ctx_cons_t *ctx, char *dest);
void console_AddToHistory(ctx_cons_t *ctx, const char *cmd);



#endif