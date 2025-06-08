
#include "test_printf.h"
#include "MeFiFo.h"


/*Fonctions utilisant le pointeur de fonction*/

// Remplie un buffer
void sputc(void *contexte, char c){
    sprintf_putc_t *ctx = (sprintf_putc_t*)contexte;

    if (ctx->index < ctx->size - 1) {
        ctx->buffer[ctx->index++] = c;
    }
}

// Envoie sur l'USART1
void USART1_putc_printf(void *contexte, char c){
    USART1_putc(c);
}

/* Fin de fonctions utilisant le pointeur de fonction */

// Utilise l'envoie sur USART1
void USART1_printf(const char *format, ...) {
    va_list va;
    va_start(va, format);
    
    tiny_format(USART1_putc_printf, NULL, format, va);
    
    va_end(va);
}

// Utilise l'envoie dans un buffer
void USART1_sprintf(char *buf, uint8_t size, const char *format, ...) {
    va_list va;
    sprintf_putc_t ctx = { .buffer = buf, .index = 0, .size = size };   // Initialisation du contexte
    va_start(va, format);  // Initialisation de va

    tiny_format(sputc, &ctx, format, va);    // Formate l'envoie des données

    va_end(va);

    ctx.buffer[ctx.index] = '\0';   // Fin de la chaine de caractère
}


uint32_t Sum(int nb, ...){
    va_list va;
    uint32_t sum = 0;

    va_start(va,nb);

    for(int i=0; i<nb; i++){
        int v = va_arg(va,uint32_t);
        sum += v;
    }
    va_end(va);
    return sum;
}

void tiny_format(fnPutc_t putc, void *contexte, const char *format, va_list va, ...) {

    format_t f;

    char binary[8];
    char hex[8];
    char d_buff[11];

    f.nb_car_format = 0;
    f.minus_sign = false;

    while (*format) {
        if (*format != '%') {
          putc(contexte,*format);
        } else {
            format++; 
            f.nb_car_format = 0;
            f.minus_sign = false;

            while (*format == '-' || (*format >= '0' && *format <= '9')){
                if (*format == '-') {
                    f.minus_sign = true;
                } else {
                    f.nb_car_format = f.nb_car_format * 10 + (*format - '0');  // Dans le cas ou le caractère suivant '%' est un chiffre
                }
                format++;
            }
            switch (*format) {
                case 'c': {  
                    char v = (char) va_arg(va, int);
                    putc(contexte,v);
                    break;
                }
                case 's': {  
                    char *s = va_arg(va, char *);
                    f.len = strlen(s);
                    f.space = f.nb_car_format > f.len ? (f.nb_car_format - f.len) : 0;

                    if (!f.space){
                        while (*s){
                            putc(contexte,*(s++));
                        }
                        goto b;
                    }

                    if (f.minus_sign == false) {

                        while (*s && ( f.nb_car_format-- > 0 )) {
                            putc(contexte,*(s++));
                        }
                        for (int i = 0; i < f.space; i++) {
                            putc(contexte,' ');
                        }
                    } else {
                        for (int i = 0; i < f.space; i++) {
                            putc(contexte,' ');
                        }
                        while (*s && ( f.nb_car_format-- > 0 )) {
                            putc(contexte,*(s++));
                        }
                    }
                    b:
                    break;
                }
                case 'b' : {
                    uint32_t b = va_arg(va, int);
                    meIntToBinary((uint8_t) b, binary);
                    SendString(binary);
                    break;
                }
                case 'x' : {
                    uint32_t x = va_arg(va, int);
                    meIntToHex((uint32_t) x, hex);
                    char * phex = hex;
                    while (*phex){
                        putc(contexte,*(phex++));
                    }
                    break;
                }
                case 'd' : {
                    uint64_t d = va_arg(va, int);
                    meItoa((uint64_t) d,d_buff);
                    char * pd = d_buff;
                    while (*pd){
                        putc(contexte,*(pd++));
                    }
                    break;
                }
                default:
                    putc(contexte,'?'); 
                    break;
            }
        }

        format++;
    }
    putc(contexte,'\0');
    va_end(va);
}


uint32_t meAtoi(const char * s){
    uint8_t buffer[10];
    uint8_t * ptr = buffer;

    uint32_t val = 0;
    uint32_t puiss = 1;
    uint8_t len = 0;

    while (*s && len < 10) {  
        *(ptr++) = *(s++) - '0';
        len++;
    }

    for (int i = len - 1; i >= 0; i--) {
        val += buffer[i] * puiss;
        puiss *= 10;
    }

    return val;
}


uint8_t meAtoiChar(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0'; 
    }
    return 0;
}

void meIntToBinary(uint8_t num, char *buffer) {
    uint8_t i, j = 0;
    uint8_t start = 0;  

    for (i = 0; i < 8; i++) {

        if ((num >> (7 - i)) & 1) {
            start = 1;  
        }

        if (start) {
            buffer[j++] = ((num >> (7 - i)) & 1) + '0';  
        }
    }

    if (j == 0) {  
        buffer[j++] = '0';  
    }
    buffer[j] = '\0';  
}

void meIntToHex(uint32_t num, char *buffer){

    const char hexChars[] = "0123456789ABCDEF";
    uint8_t i;

    for (i=0; i<2*sizeof(num); i++){
        buffer[i] = hexChars[(num >> (32 - 4)) & 0x0F];
        num <<= 4;
    }
    buffer[i]= '\0';
}


void meItoa(uint32_t num,char * buff){

    uint8_t i = 0;
    
    if (num == 0) {  
        buff[i++] = '0';  
    } else {
        while (num > 0) {
            buff[i++] = (num % 10) + '0'; 
            num /= 10; 
        }
    }

    buff[i] = '\0';  

    for (uint8_t j = 0; j < i / 2; j++) {
        char temp = buff[j];
        buff[j] = buff[i - 1 - j];
        buff[i - 1 - j] = temp;
    }
}

