#ifndef TEST_PRINTF_H
#define TEST_PRINTF_H


#include "main.h"
#include "meUSART1.h"
#include <stdarg.h>

typedef struct {
    bool minus_sign;   // Si je recois un signe - ex : "james %-10"
    uint8_t len;       // Longueur de la chaine de caractère à envoyer
    uint8_t space;      // Ecart entre le nombre de caractère du format (%10) et la longueur de la chaine
    uint8_t nb_car_format;    
}format_t;

typedef struct {
    char * buffer;
    uint8_t index;
    uint16_t size;
}sprintf_putc_t;

typedef void (*fnPutc_t)(void *, char c);

uint32_t Sum(int nb, ...);
uint32_t meAtoi(const char * s);
uint8_t meAtoiChar(const char c);
void meIntToBinary(uint8_t num, char *buffer);
void meIntToHex(uint32_t num, char *buffer);
void meItoa(uint32_t num, char * buffer);


void sputc(void *contexte, char c);
void USART1_putc_printf(void * contexte, char c);

void tiny_format(fnPutc_t putc, void *contexte, const char *format, va_list va, ...);

void USART1_printf(const char *format, ...);
void USART1_sprintf(char *buf, uint8_t size, const char *format, ...);

#endif