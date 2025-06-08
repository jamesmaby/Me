
#include "meConsoleCommande.h"
#include "meConsole.h"



// cons_cmd_rc_t console_fn_Help(ctx_cons_t *ctx){

//     if (ctx->argc > 1) {
//         for (uint8_t j = 0; com_cmd_desc[j].fnCon != NULL; j++) { 
//             if (strcmp(ctx->argv[1], com_cmd_desc[j].name) == 0) {
//                 PRINT("\r\033[36m%10s %40s %s \r\n","| name |","|descritpion|","|usage|");
//                 PRINT("%10s %40s  %s \r\n\033[0m",com_cmd_desc[j].name,com_cmd_desc[j].desc,com_cmd_desc[j].usage);
//                 console_Prompt(ctx);
//                 return CON_RC_DONE;
//             } else continue;
//         }
//         PRINT("argument inconnu : %s\r\n", ctx->argv[1]);
//         console_Prompt(ctx);
//         return CON_RC_BAD_ARG;
//     }
    
//     PRINT("\033[36m");
//     PRINT("%10s %40s %s \r\n","| name |","|descritpion|","|usage|");
//     for (uint8_t j = 0; com_cmd_desc[j].fnCon != NULL; j++) { 
//         PRINT("%10s %40s  %s \r\n",com_cmd_desc[j].name,com_cmd_desc[j].desc,com_cmd_desc[j].usage);
//     }
//     PRINT("\033[0m");
//     console_Prompt(ctx);
//     return CON_RC_DONE;
// }

// cons_cmd_rc_t console_fn_Test(ctx_cons_t *ctx){
//     PRINT("\033[36m%s\r\n\033[0m",com_cmd_desc[TEST].desc);
//     console_Prompt(ctx);
//     return CON_RC_DONE;
// }

// cons_cmd_rc_t console_fn_Uptime(ctx_cons_t *ctx) {
    
//     if (ctx->is_interactive) goto interactive;

//     typedef struct {
//         delay_t uptime_delay;
//         uint64_t uptime_timeout;
//     } con_ctx_uptime_t;

//     CTX_FN(ctx,con_ctx_uptime_t);
//     p->uptime_timeout = 1000;

//     bool format_flag = false;

//     for (int i = 1; i < ctx->argc; i++) {  // Commence à 1 (ignore la commande elle-même)
//         if (strcmp(ctx->argv[i], "-f") == 0) {
//             format_flag = true;
//         } else {
//             PRINT("argument inconnu : %s\r\n", ctx->argv[i]);
//             console_Prompt(ctx);
//             return CON_RC_BAD_ARG;
//         }
//     }

//     meDelayInit(&p->uptime_delay,p->uptime_timeout);

//     if (format_flag) {
//         if (ctx->is_interactive == 0){
//             ctx->is_interactive = true;
//             ctx->fn_cmd = console_fn_Uptime;
//         }else{
//             interactive:
//             con_ctx_uptime_t *p = (con_ctx_uptime_t *) ctx->ctx_mem;
//             if (meDelay(&(p->uptime_delay))){
//                 PRINT("\rJour : %d, Heure : %d, Minutes : %d, Secondes : %d \r",GiveTimeSec()/(24*3600),(GiveTimeSec()/3600)%24, (GiveTimeSec()/60)%60, GiveTimeSec()%60);
//                 meDelayInit(&(p->uptime_delay),p->uptime_timeout);
//             } else return INTERACTIVE;
//         }
//     } else {
//         PRINT("\rJour : %d, Heure : %d, Minutes : %d, Secondes : %d \r\n",GiveTimeSec()/(24*3600),(GiveTimeSec()/3600)%24, (GiveTimeSec()/60)%60, GiveTimeSec()%60);
//         console_Prompt(ctx);
//     }

//     return INTERACTIVE;
// }

// cons_cmd_rc_t console_fn_Reboot(ctx_cons_t *ctx) {
//     NVIC_SystemReset();
//     console_Prompt(ctx);
//     return CON_RC_DONE;
// }

// cons_cmd_rc_t console_fn_Echo(ctx_cons_t *ctx) {
//     bool newline = 0;
//     uint8_t start_index = 1;

//     if (ctx->argc == 1) {
//         PRINT("\r\n"); 
//         return CON_RC_DONE;
//     }

//     if(!strcmp(ctx->argv[start_index],"-n")){
//         newline = true;
//         start_index = 2;
//     }

//     for (int i = start_index; i < ctx->argc; i++) {
//         PRINT("%s", ctx->argv[i]);
//         if (i < ctx->argc - 1) {
//             PRINT(" "); 
//         }
//     }
//     if (!newline){
//         PRINT("\r\n"); 
//         console_Prompt(ctx);
//     } 

//     return CON_RC_DONE;
// }

// cons_cmd_rc_t console_fn_Pong(ctx_cons_t *ctx) {

//     PRINT("ALLEZ LE FOOT\r\n");
//     console_Prompt(ctx);
//     return CON_RC_DONE;
// }

// cons_cmd_rc_t console_fn_Clear(ctx_cons_t *ctx) {
//     console_Clear(ctx);
//     console_Prompt(ctx);
//     return CON_RC_DONE;
// }