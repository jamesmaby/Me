#include "meConsole.h"
#include "meConsoleCarac.h"

#define PRINT(...) me_sd_Printf(ctx->pme_sd, __VA_ARGS__)

#define DEF_SEQ( _seq, _code) { _seq, sizeof(_seq) - 1, _code }

#define CTX_FN(_ctx, _ctx_fn)                                      \
if (sizeof(_ctx_fn) > sizeof(_ctx->ctx_mem)) {               \
    PRINT("Erreur: mémoire insuffisante pour le contexte\r\n");\
    return CON_RC_DONE;                                        \
}                                                              \
_ctx_fn *p = (_ctx_fn *) _ctx->ctx_mem; 


#define SEQUENCE \
const cons_sp_char_t cons_sp_char[] = { \
    DEF_SEQ( "\x1B[D", LEFT_ARROW), \
    DEF_SEQ( "\x1B[C", RIGHT_ARROW), \
    DEF_SEQ( "\x1B[A", UP_ARROW), \
    DEF_SEQ( "\x1B", ECHAP), \
    DEF_SEQ( "\x1B[4~", FIN), \
    DEF_SEQ( "\x1B[1~", DEBUT), \
    DEF_SEQ( "\x1B[3~", DEL), \
    DEF_SEQ( NULL, NO_INPUT),  \
};

extern const com_cmd_desc_t __start_console_cmd_list;
extern const com_cmd_desc_t __end_console_cmd_list;


#define COMMANDE(_id,_name, _desc, _usage, _function) \
    __attribute__((section(".console_cmd_list"), aligned(4))) \
    const com_cmd_desc_t console_cmd_##_id = { \
        .name = _name, \
        .desc = _desc, \
        .usage = _usage, \
        .fnCon = _function \
        }


COMMANDE(help, "help", "Donne toutes les commandes possibles", "help [command]", console_fn_Help);
COMMANDE(test, "test", "Ceci est une fonction test", "test", console_fn_Test);
COMMANDE(uptime, "uptime", "Donne le temps écoulé depuis le démarrage système", "uptime [-f]", console_fn_Uptime);
COMMANDE(reboot, "reboot", "Reboot le système", "reboot", console_fn_Reboot);
COMMANDE(echo, "echo", "Affiche le texte sur le terminal", "echo", console_fn_Echo);
COMMANDE(clear, "clear", "Clear la console", "clear", console_fn_Clear);
COMMANDE(pong, "pong", "Voici le fonctionnement de pong", "pong", console_fn_Pong);
    
SEQUENCE


void console_Init(ctx_cons_t *pctx_cons, me_sd_t *pme_sd){

    pctx_cons->pme_sd = pme_sd;

    pctx_cons->is_esc = 0;
    pctx_cons->is_print = 0;
    pctx_cons->current_size = 0;
    pctx_cons->index = 0;
    pctx_cons->index_seq = 0;
    pctx_cons->argc = 0;
    pctx_cons->is_interactive = 0;

    pctx_cons->history_index = 0;  
    
    for (int i = 0; i < CON_ARGC_MAX; i++) {
        pctx_cons->argv[i] = NULL;
    }

    meDelayDisable(pctx_cons->delay_cons);
    console_Clear(pctx_cons);
    console_Prompt(pctx_cons);
}

void console_polling(ctx_cons_t *ctx) {
    char c = console_Reading(ctx);

    if (c == NO_INPUT){
        if (ctx->is_interactive) {
            com_cmd_desc_t *com_cmd_desc = (com_cmd_desc_t *) ctx->com_cmd_desc;
            com_cmd_desc->fnCon(ctx);
        }
        return;
    }
    switch (c) {
        case ECHAP:
            console_Echap(ctx);
            break;
        case '\r':
            if (console_entrer(ctx)) console_match_cmd(ctx);
            break;
        case '\177':
            console_Backspace(ctx);
            break;
        case LEFT_ARROW:
            console_LeftArrow(ctx);
            break;
        case RIGHT_ARROW:
            console_RightArrow(ctx);
            break;
        case UP_ARROW:
            console_UpArrow(ctx);
            break;
        case DEBUT:
            console_Home(ctx);
            break;
        case FIN:
            console_End(ctx);
            break;
        case DEL:
            console_Del(ctx);
            break;
        default:
            console_Carac(ctx, c);
            break;
        }
}

char console_Reading(ctx_cons_t *ctx) {
    
    char c;

    if (!me_sd_Read(ctx->pme_sd, &c, 1)) {    // Tant que je ne recois rien
        
        if (ctx->is_esc && meDelay(ctx->delay_cons)) {  // si etat d'échappement + timeout 
            ctx->is_esc = 0; // Reset etat d'échappement
            if (ctx->index_seq == 1){     // Dans le cas de '\x1B'
                goto seq;
            }
            ctx->index_seq = 0;
        }
        return NO_INPUT;
    }
    // PRINT("%x \r\n",c);

    if (c == '\x1B') {
        ctx->buff_seq[0] = c;
        ctx->index_seq = 1;
        ctx->is_esc = 1;
        meDelayInit(ctx->delay_cons, TIMEOUT); 
        return NO_INPUT;  
    }
    

    if (ctx->is_esc) {
        ctx->buff_seq[ctx->index_seq++] = c;
        seq:
       for (uint8_t i = 0; cons_sp_char[i].pSeq != NULL; i++){
            if (memcmp(ctx->buff_seq, cons_sp_char[i].pSeq, ctx->index_seq) == 0) {
                if (ctx->index_seq == cons_sp_char[i].len) {
                    ctx->is_esc = 0;
                    ctx->index_seq = 0;
                    return cons_sp_char[i].code;
                }
                if (ctx->index_seq == 1){
                    continue;
                }else{return NO_INPUT;}
            }
        }
        ctx->is_esc = 0;
        ctx->index_seq = 0;
        return NO_INPUT;
    }
    
    return c;
}

void console_Prompt(ctx_cons_t *ctx){

    me_sd_Print(ctx->pme_sd, "> ");
}

cons_cmd_rc_t console_parse_cmd(ctx_cons_t *ctx) {

    char *start = ctx->buffer;  
    char **pav = ctx->argv;  
    ctx->argc = 0; 

    *(pav++) = start;

    if ((*start) == '\0' || (*start) == ' '){
        ctx->current_size = 0;
        return CON_RC_BAD_ARG;
    }
    ctx->argc++; 

    while (*start){
        if (*start == ' '){
            *start = '\0';
            if (ctx->argc < CON_ARGC_MAX ){
                ctx->argc++; 
                start++;
                *(pav++) = start;
            }
            continue;
        }
        start++;
        continue;
    }

    ctx->current_size = 0;
    return CON_RC_DONE;
}

cons_cmd_rc_t console_match_cmd(ctx_cons_t *ctx) {

    char **pav = ctx->argv;
    
    console_parse_cmd(ctx);

    const com_cmd_desc_t *cmd = &__start_console_cmd_list;
    const com_cmd_desc_t *cmd_end = &__end_console_cmd_list;

    for (int i = 0; i < ctx->argc; i++) {

        while (cmd < cmd_end) {
            if (cmd->fnCon != NULL && !strcmp(*pav, cmd->name)) {
                ctx->com_cmd_desc = cmd;
                return cmd->fnCon(ctx);
            }
            cmd++;
        }
    }
    PRINT("\rCommande introuvable, utilisez 'help'\r\n");
    console_Prompt(ctx);
    return CON_RC_BAD_ARG;
}


cons_cmd_rc_t console_fn_Help(ctx_cons_t *ctx){
    const com_cmd_desc_t *cmd = &__start_console_cmd_list;
    const com_cmd_desc_t *cmd_end = &__end_console_cmd_list;

    if (ctx->argc > 1) {
        while (cmd < cmd_end) {
            if (cmd->fnCon != NULL && strcmp(ctx->argv[1], cmd->name) == 0) {
                PRINT("\r\033[36m%10s %60s %s \r\n","| name |","|descritpion|","|usage|");
                PRINT("%10s %40s  %s \r\n\033[0m",cmd->name, cmd->desc, cmd->usage);
                console_Prompt(ctx);
                return CON_RC_DONE;
            } else continue;
        }
        PRINT("argument inconnu : %s\r\n", ctx->argv[1]);
        console_Prompt(ctx);
        return CON_RC_BAD_ARG;
    }
    
    PRINT("\033[36m");
    PRINT("%10s %60s %s \r\n","| name |","|descritpion|","|usage|");

    cmd = &__start_console_cmd_list;
    while (cmd < cmd_end) {
        if (cmd->fnCon != NULL) {
            PRINT("%10s %60s  %s \r\n", cmd->name, cmd->desc, cmd->usage);
        }
        cmd++;
    }
    PRINT("\033[0m");
    console_Prompt(ctx);
    return CON_RC_DONE;
}

cons_cmd_rc_t console_fn_Test(ctx_cons_t *ctx){
    com_cmd_desc_t *com_cmd_desc = (com_cmd_desc_t *) ctx->com_cmd_desc;

    PRINT("\033[36m%s\r\n\033[0m",com_cmd_desc->desc);
    console_Prompt(ctx);
    return CON_RC_DONE;
}

cons_cmd_rc_t console_fn_Uptime(ctx_cons_t *ctx) {
    
    if (ctx->is_interactive) goto interactive;

    typedef struct {
        delay_t uptime_delay;
        uint64_t uptime_timeout;
    } con_ctx_uptime_t;

    CTX_FN(ctx,con_ctx_uptime_t);
    p->uptime_timeout = 1000;

    bool format_flag = false;

    for (int i = 1; i < ctx->argc; i++) {  // Commence à 1 (ignore la commande elle-même)
        if (strcmp(ctx->argv[i], "-f") == 0) {
            format_flag = true;
        } else {
            PRINT("argument inconnu : %s\r\n", ctx->argv[i]);
            console_Prompt(ctx);
            return CON_RC_BAD_ARG;
        }
    }

    meDelayInit(&p->uptime_delay,p->uptime_timeout);

    if (format_flag) {
        if (ctx->is_interactive == 0){
            ctx->is_interactive = true;
        }else{
            interactive:
            con_ctx_uptime_t *p = (con_ctx_uptime_t *) ctx->ctx_mem;
            if (meDelay(&(p->uptime_delay))){
                PRINT("\rJour : %d, Heure : %d, Minutes : %d, Secondes : %d \r",GiveTimeSec()/(24*3600),(GiveTimeSec()/3600)%24, (GiveTimeSec()/60)%60, GiveTimeSec()%60);
                meDelayInit(&(p->uptime_delay),p->uptime_timeout);
            } else return INTERACTIVE;
        }
    } else {
        PRINT("\rJour : %d, Heure : %d, Minutes : %d, Secondes : %d \r\n",GiveTimeSec()/(24*3600),(GiveTimeSec()/3600)%24, (GiveTimeSec()/60)%60, GiveTimeSec()%60);
        console_Prompt(ctx);
    }

    return INTERACTIVE;
}

cons_cmd_rc_t console_fn_Reboot(ctx_cons_t *ctx) {
    NVIC_SystemReset();
    console_Prompt(ctx);
    return CON_RC_DONE;
}

cons_cmd_rc_t console_fn_Echo(ctx_cons_t *ctx) {
    bool newline = 0;
    uint8_t start_index = 1;

    if (ctx->argc == 1) {
        PRINT("\r\n"); 
        return CON_RC_DONE;
    }

    if(!strcmp(ctx->argv[start_index],"-n")){
        newline = true;
        start_index = 2;
    }

    for (int i = start_index; i < ctx->argc; i++) {
        PRINT("%s", ctx->argv[i]);
        if (i < ctx->argc - 1) {
            PRINT(" "); 
        }
    }
    if (!newline){
        PRINT("\r\n"); 
        console_Prompt(ctx);
    } 

    return CON_RC_DONE;
}

cons_cmd_rc_t console_fn_Pong(ctx_cons_t *ctx) {

    PRINT("ALLEZ LE FOOT\r\n");
    console_Prompt(ctx);
    return CON_RC_DONE;
}

cons_cmd_rc_t console_fn_Clear(ctx_cons_t *ctx) {
    console_Clear(ctx);
    console_Prompt(ctx);
    return CON_RC_DONE;
}