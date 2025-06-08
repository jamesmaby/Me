#include "meConsoleCarac.h"


void console_Clear(ctx_cons_t *ctx){
    char reposition[10];
    USART1_sprintf(reposition, sizeof(reposition), "\x1B[2J\x1B[H");
    me_sd_Print(ctx->pme_sd, reposition);

}

bool console_entrer(ctx_cons_t *ctx){

    if (ctx->is_interactive) return false;

    if (ctx->index == 0) {
        ctx->buffer[ctx->index] = '\0';
        me_sd_Printf(ctx->pme_sd,"\r\n");
        console_Prompt(ctx);
        return false;
    }


    // memcpy(&ctx->back_seq_buff[ctx->back_head],ctx->buffer,ctx->current_size);
    // ctx->back_head += ctx->current_size % CON_BACK_SIZE;

    // strcpy(ctx->back_seq[ctx->index_back_seq++ & (CON_BACK_SIZE-1)],ctx->buffer);

    me_sd_Printf(ctx->pme_sd,"\r\n%s\r\n",ctx->buffer);
    console_Prompt(ctx);

    ctx->index = 0; 
    
    return true;
}

void console_Backspace(ctx_cons_t *ctx){

    if (ctx->index > 0) {
        ctx->index--;
        ctx->current_size--;

        memmove(&ctx->buffer[ctx->index], &ctx->buffer[ctx->index + 1], ctx->current_size - ctx->index +1);
        ctx->buffer[ctx->current_size] = '\0';

        me_sd_Print(ctx->pme_sd, "\r\x1B[K");
        console_Prompt(ctx);
        me_sd_Print(ctx->pme_sd, ctx->buffer);

        char reposition[10];
        USART1_sprintf(reposition, sizeof(reposition),"\x1B[%dG", (ctx->index+3));
        me_sd_Print(ctx->pme_sd, reposition);
    }

}

void console_LeftArrow(ctx_cons_t *ctx){

    if (ctx->index > 0) {
        ctx->index--;
        char reposition[10];
        USART1_sprintf(reposition, sizeof(reposition), "\x1B[%dG", (int)ctx->index + 3);
        me_sd_Print(ctx->pme_sd, reposition);
    }
}

void console_RightArrow(ctx_cons_t *ctx){

    if (ctx->index < ctx->current_size) {
        ctx->index++;

        char reposition[10];
        USART1_sprintf(reposition,sizeof(reposition), "\x1B[%dG", (int)(ctx->index + 3) );
        me_sd_Print(ctx->pme_sd, reposition);
    }
}

void console_UpArrow(ctx_cons_t *ctx) {
    if (ctx->is_interactive) return;

    me_sd_Printf(ctx->pme_sd, "\r\x1B[K");

    if (!console_GetPreviousCommand(ctx, ctx->buffer)) {
        return;
    }

    console_Prompt(ctx);
    me_sd_Printf(ctx->pme_sd, "%s", ctx->buffer);

    // Repositionner le curseur
    char reposition[10];
    USART1_sprintf(reposition, sizeof(reposition), "\x1B[%dG", (int)(strlen(ctx->buffer) + 3));
    me_sd_Print(ctx->pme_sd, reposition);

    ctx->index = strlen(ctx->buffer);
    ctx->current_size = ctx->index;
}


void console_Echap(ctx_cons_t *ctx){
    ctx->is_interactive = false;

    ctx->current_size = 0;
    ctx->index = 0;
    me_sd_Print(ctx->pme_sd, "\r\n");
    console_Prompt(ctx);

}

void console_Home(ctx_cons_t *ctx){
    ctx->index = 0;

    char reposition[10];
    USART1_sprintf(reposition, sizeof(reposition), "\x1B[%dG", 2);
    me_sd_Print(ctx->pme_sd, reposition);

}

void console_End(ctx_cons_t *ctx){
    ctx->index = ctx->current_size;

    char reposition[10];
    USART1_sprintf(reposition, sizeof(reposition), "\x1B[%dG",ctx->current_size+3);
    me_sd_Print(ctx->pme_sd, reposition);


}

void console_Carac (ctx_cons_t *ctx, char c){
    if (ctx->is_interactive){
        return;
    }

    if (ctx->index < (CONS_BUFFER_SIZE - 1) && ctx->current_size < (CONS_BUFFER_SIZE)) {
        // Décaler les caractères à droite de l'index pour insérer un nouveau caractère
        memmove(&ctx->buffer[(ctx->index + 1) & (CONS_BUFFER_SIZE - 1)], &ctx->buffer[ctx->index], ctx->current_size - ctx->index);
        ctx->buffer[ctx->index] = c;
        ctx->index++;
        ctx->index &= CONS_BUFFER_SIZE - 1;
        ctx->current_size++;
        ctx->buffer[ctx->current_size] = '\0';

        me_sd_Print(ctx->pme_sd, "\r\x1B[K");
        console_Prompt(ctx);
        // strcpy(temp,ctx->buffer);
        me_sd_Print(ctx->pme_sd, ctx->buffer); 

        char reposition[10];
        USART1_sprintf(reposition, sizeof(reposition), "\x1B[%dG", ctx->index + 3);
        me_sd_Print(ctx->pme_sd, reposition);
    }
}

void console_Del(ctx_cons_t *ctx){

    if (ctx->index < ctx->current_size - 1 && ctx->current_size < (CONS_BUFFER_SIZE - 2)) {
        ctx->current_size--;

        memmove(&ctx->buffer[ctx->index], &ctx->buffer[ctx->index + 1], ctx->current_size - ctx->index);
        ctx->buffer[ctx->current_size] = '\0';

        me_sd_Print(ctx->pme_sd, "\r\x1B[K");
        console_Prompt(ctx);  
        me_sd_Print(ctx->pme_sd, ctx->buffer);

        char reposition[10];
        USART1_sprintf(reposition, sizeof(reposition),"\x1B[%dD", (ctx->current_size - ctx->index) );
        me_sd_Print(ctx->pme_sd, reposition);
    }

}
void console_AddToHistory(ctx_cons_t *ctx, const char *cmd) {

    // memmove(ctx->buffer[ctx->index],ctx->buffer,strlen(cmd) + 1);

}


bool console_GetPreviousCommand(ctx_cons_t *ctx, char *dest) {

    uint8_t i = 1;
    while (ctx->buffer++){
        i++;
    }
    memmove(dest,ctx->buffer+i,strlen(ctx->buffer+i)+1);

    return true;
}
