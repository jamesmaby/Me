#include "meConsoleCarac.h"
#include "meConsole.h"


void console_Clear(conCtx_t *ctx){
    char reposition[10];
    meFd_Sprintf(reposition, sizeof(reposition), "\x1B[2J\x1B[H");
    meFd_Print(ctx->pme_sd, reposition);

}

bool console_entrer(conCtx_t *ctx){

    if (ctx->is_interactive) return false;

    if (ctx->index == 0) 
    {
        ctx->buffer[0] = '\0';
        meFd_Printf(ctx->pme_sd,"\r\n");
        console_Prompt(ctx);
        return false;
    }


    memcpy(ctx->command_history[ctx->history_read_index], ctx->buffer, ctx->current_size);
    ctx->command_lengths[ctx->history_read_index] = ctx->current_size; // Sauvegarde la taille de la commande
    ctx->history_write_index = ctx->history_read_index; //  
    ctx->history_read_index++;
    ctx->history_read_index &= (CONS_MAX_COMMANDE_HISTORY - 1); // modulo

    meFd_Printf(ctx->pme_sd,"\r\n%s\r\n",ctx->buffer);
    console_Prompt(ctx);

    ctx->index = 0; 
    
    return true;
}

void console_Backspace(conCtx_t *ctx){

    if (ctx->index > 0) {
        ctx->index--;
        ctx->current_size--;

        memmove(&ctx->buffer[ctx->index], &ctx->buffer[ctx->index + 1], ctx->current_size - ctx->index +1);
        ctx->buffer[ctx->current_size] = '\0';

        console_ClearLine(ctx);
        console_Prompt(ctx);
        meFd_Print(ctx->pme_sd, ctx->buffer);

        char reposition[10];
        meFd_Sprintf(reposition, sizeof(reposition),"\x1B[%dG", (ctx->index+LEN_PROMPT));
        meFd_Print(ctx->pme_sd, reposition);
    }

}

void console_LeftArrow(conCtx_t *ctx){

    if (ctx->index > 0) {
        ctx->index--;
        char reposition[10];
        meFd_Sprintf(reposition, sizeof(reposition), "\x1B[%dG", (int)ctx->index + LEN_PROMPT);
        meFd_Print(ctx->pme_sd, reposition);
    }
}

void console_RightArrow(conCtx_t *ctx){

    if (ctx->index < ctx->current_size) {
        ctx->index++;

        char reposition[10];
        meFd_Sprintf(reposition,sizeof(reposition), "\x1B[%dG", (int)(ctx->index + LEN_PROMPT) );
        meFd_Print(ctx->pme_sd, reposition);
    }
}

void console_UpArrow(conCtx_t *ctx) {
    if (ctx->is_interactive) return;
    uint8_t idx = ctx->history_write_index;

    if ( ctx->command_history[idx][0] == 0 ) // Si je n'ai pas de commande en mémoire
        return;

    console_ClearLine(ctx);
    memcpy(ctx->buffer, ctx->command_history[idx], ctx->command_lengths[idx]); // Copier la commande
    ctx->buffer[ctx->command_lengths[idx]] = '\0'; // Fin de chaîne sinon pb si taille précédente > taille actuelle
    ctx->current_size = ctx->command_lengths[idx]; // Màj taille courante
    ctx->history_write_index = (idx - 1) & (CONS_MAX_COMMANDE_HISTORY - 1); // modulo

    console_Prompt(ctx);
    meFd_Printf(ctx->pme_sd, "%s", ctx->buffer);

    // Repositionner le curseur
    char reposition[10];
    meFd_Sprintf(reposition, sizeof(reposition), "\x1B[%dG", (int)(strlen(ctx->buffer) + LEN_PROMPT));
    meFd_Print(ctx->pme_sd, reposition);

    ctx->index = strlen(ctx->buffer);
    ctx->current_size = ctx->index;
}

void console_DownArrow(conCtx_t *ctx) {
    if (ctx->is_interactive) return;
    uint8_t idx = ctx->history_write_index;

    if ( ctx->command_history[idx][0] == 0 ) // Si je n'ai pas de commande en mémoire | ou si je suis à la commande la plus récente
        return;

    idx = (idx + 1) & (CONS_MAX_COMMANDE_HISTORY - 1); // modulo
    if ( ctx->command_history[idx][0] == 0 ) // Si je n'ai pas de commande en mémoire
        return;


    console_ClearLine(ctx);
    memcpy(ctx->buffer, ctx->command_history[idx], ctx->command_lengths[idx]); // Copier la commande
    ctx->buffer[ctx->command_lengths[idx]] = '\0'; // Fin de chaîne sinon pb si taille précédente > taille actuelle
    ctx->current_size = ctx->command_lengths[idx]; // Màj taille courante
    ctx->history_write_index = idx;

    console_Prompt(ctx);
    meFd_Printf(ctx->pme_sd, "%s", ctx->buffer);

    // Repositionner le curseur
    char reposition[10];
    meFd_Sprintf(reposition, sizeof(reposition), "\x1B[%dG", (strlen(ctx->buffer) + LEN_PROMPT));
    meFd_Print(ctx->pme_sd, reposition);

    ctx->index = strlen(ctx->buffer);
    ctx->current_size = ctx->index;
}


void console_Echap(conCtx_t *ctx){
    ctx->is_interactive = false;

    ctx->current_size = 0;
    ctx->index = 0;
    meFd_Print(ctx->pme_sd, "\r\n");
    console_Prompt(ctx);

}

void console_Home(conCtx_t *ctx){
    ctx->index = 0;

    char reposition[10];
    meFd_Sprintf(reposition, sizeof(reposition), "\x1B[%dG", 2);
    meFd_Print(ctx->pme_sd, reposition);

}

void console_End(conCtx_t *ctx){
    ctx->index = ctx->current_size;

    char reposition[10];
    meFd_Sprintf(reposition, sizeof(reposition), "\x1B[%dG",ctx->current_size+LEN_PROMPT);
    meFd_Print(ctx->pme_sd, reposition);


}

void console_Carac (conCtx_t *ctx, char c){
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

        console_ClearLine(ctx);
        console_Prompt(ctx);
        meFd_Print(ctx->pme_sd, ctx->buffer); 

        char reposition[10];
        meFd_Sprintf(reposition, sizeof(reposition), "\x1B[%dG", ctx->index + LEN_PROMPT);
        meFd_Print(ctx->pme_sd, reposition);
    }
}

void console_Del(conCtx_t *ctx){

    if (ctx->index < ctx->current_size - 1 && ctx->current_size < (CONS_BUFFER_SIZE - 2)) {
        ctx->current_size--;

        memmove(&ctx->buffer[ctx->index], &ctx->buffer[ctx->index + 1], ctx->current_size - ctx->index);
        ctx->buffer[ctx->current_size] = '\0';

        console_ClearLine(ctx);
        console_Prompt(ctx);  
        meFd_Print(ctx->pme_sd, ctx->buffer);

        char reposition[10];
        meFd_Sprintf(reposition, sizeof(reposition),"\x1B[%dD", (ctx->current_size - ctx->index) );
        meFd_Print(ctx->pme_sd, reposition);
    }
}

void console_Tabulation(conCtx_t *ctx){
    if (ctx->current_size == 0) return;
    const com_cmd_desc_t* p =  &__start_console_cmd_list;
    const com_cmd_desc_t* pend =  &__end_console_cmd_list;
    bool found = false;

    while ( p != pend )
    {
        if ( !strncmp(p->name, ctx->buffer, ctx->current_size) ){
            found = true;
            break;
        }
        p++;
    }

    if (found){
        console_ClearLine(ctx);
        strcpy(ctx->buffer, p->name);
        ctx->current_size = strlen(p->name);
        ctx->index = ctx->current_size;
        console_Prompt(ctx);
        meFd_Print(ctx->pme_sd, ctx->buffer);

        char reposition[10];
        meFd_Sprintf(reposition, sizeof(reposition), "\x1B[%dG", (ctx->index + LEN_PROMPT));
        meFd_Print(ctx->pme_sd, reposition);
    }
}

