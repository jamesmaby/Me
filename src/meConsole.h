#ifndef __MECONSOLE_H
#define __MECONSOLE_H

#include "meFd.h"
#include "meDelay.h"
#include "mePrintf.h"
#include "main.h"


#define CONS_BUFFER_SIZE 64
#define TIMEOUT 50
#define CON_ARGC_MAX 8
#define CON_BACK_BUFFER_SIZE 64
#define CONS_MAX_COMMANDE_HISTORY 4// 8 dernières commandes en mémoire
#define LEN_PROMPT strlen(promptName)+2+1 // Taille du prompt + "> " + 1 pour décaler le curseur

#ifndef CON_CTX_CMD
    #define CON_CTX_CMD 4
#endif

#define DEF_CONS(_name, _size)                  \
        static char buff_cons_##_name[_size];          \
        static char buff_seq_##_name[6];              \
        conCtx.buff_seq = buff_seq_##_name;     \
        conCtx.buffer = buff_cons_##_name;      \
        static meDelay_t cons_delay_##_name;    \
        conCtx.delay_cons = &cons_delay_##_name;

#define COMMANDE(_id, _name, _function, _desc, _usage)          \
    __attribute__((section(".console_cmd_list"), aligned(4)))   \
    const com_cmd_desc_t console_cmd_##_id = {                  \
        .name = _name,          \
        .desc = _desc,          \
        .usage = _usage,        \
        .fnCon = _function      \
    }


typedef enum {
    CON_RC_DONE,
    CON_RC_BAD_ARG,
    INTERACTIVE,
}cons_cmd_rc_t;


typedef struct {

    meFd_t *pme_sd;

    char *buffer;
    uint16_t index;
    uint16_t current_size;

    uint16_t size_mem;

    uint8_t is_esc;
    uint8_t is_print;
    uint8_t index_seq;
    char *buff_seq;
    meDelay_t *delay_cons;

    char* argv[CON_ARGC_MAX];
    uint8_t argc;

    const void *com_cmd_desc;

    void *ctx_mem[CON_CTX_CMD];
    bool is_interactive;

    uint8_t command_lengths[CONS_MAX_COMMANDE_HISTORY];
    char command_history[CONS_MAX_COMMANDE_HISTORY][CON_BACK_BUFFER_SIZE];
    uint8_t history_write_index;
    uint8_t history_read_index;

}conCtx_t;

extern conCtx_t conCtx;
#define printf(...) meFd_Printf(conCtx.pme_sd, __VA_ARGS__)

typedef struct {
    const char* name;
    const char* desc;
    const char* usage;
    cons_cmd_rc_t(*fnCon)(conCtx_t *ctx);
}com_cmd_desc_t;

extern const com_cmd_desc_t __start_console_cmd_list;
extern const com_cmd_desc_t __end_console_cmd_list;


typedef struct {
    const char* pSeq;
    uint8_t len;
    char code;
}cons_sp_char_t;

typedef enum {
    NO_INPUT,
    LEFT_ARROW,
    RIGHT_ARROW,
    UP_ARROW,
    DOWN_ARROW,
    TABULATION,
    DEBUT,
    FIN,
    ECHAP,
    DEL = 10,
}enum_t;



typedef enum {
    HELP,
    TEST,
    UPTIME,
    REBOOT,
    ECHO,
    CLEAR,
    PONG,
    END,
}com_enum_t;




void console_Init(conCtx_t *ctx, meFd_t *pme_sd);
void console_Polling(conCtx_t *ctx);
char console_Reading(conCtx_t *ctx);
void console_Prompt(conCtx_t *ctx);
void console_ClearLine(conCtx_t *ctx);


cons_cmd_rc_t console_fn_Uptime(conCtx_t *ctx);
cons_cmd_rc_t console_fn_Help(conCtx_t *ctx);
cons_cmd_rc_t console_fn_Test(conCtx_t *ctx);
cons_cmd_rc_t console_fn_Reboot(conCtx_t *ctx);
cons_cmd_rc_t console_fn_Echo(conCtx_t *ctx);
cons_cmd_rc_t console_fn_Clear(conCtx_t *ctx);
cons_cmd_rc_t console_fn_Pong(conCtx_t *ctx);


cons_cmd_rc_t console_parse_cmd(conCtx_t *ctx);
cons_cmd_rc_t console_match_cmd(conCtx_t *ctx);




#endif