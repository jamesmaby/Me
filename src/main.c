#include "stm32f0xx.h"
#include "system.h"
#include "io.h"

#include "main.h"
#include "meDelay.h"
#include "MeFiFo.h"
#include "meUSART1.h"
#include "test_printf.h"
#include "meFileDescriptor.h"
#include "meUSARTFD.h"
#include "meFIFOFD.h"
#include "meConsole.h"

#define DEF_CONS(_name, _size) ctx_cons_t ctx_cons; \
char buff_cons_##_name[_size]; \
char buff_seq_##_name [6]; \
ctx_cons.buff_seq = buff_seq_##_name; \
ctx_cons.buffer = buff_cons_##_name; \
static delay_t cons_delay_##_name; \
ctx_cons.delay_cons = &cons_delay_##_name;


uint8_t	rx_byte;
char	rx_tab[BUFFER_SIZE];

extern uint8_t indice;

uint32_t birthday = 200;
delay_t delay_main;

char receive_fifo;

DEFINE_FIFO(fifo_Rx,FIFO_RX_SIZE) 
DEFINE_FIFO(fifo_Tx,FIFO_TX_SIZE) 


int main(void)
{
	System_Init();
	
	// UART_Init();
	// NVIC_Initialisation();
	
	meDelayInit(&delay_main,birthday);

	me_sd_t me_sd;

	DEF_CONS(cons_1, 64);

	fd_FIFO_USART1_Init(&me_sd,&fifo_Rx,&fifo_Tx);
	console_Init(&ctx_cons,&me_sd);

    while(1)
    {
		char buffer[64];
		strcpy(buffer, "bonjour");  // Initialise le buffer proprement
	
		console_polling(&ctx_cons);

		if( meDelay(&delay_main) == 1 ){
			meDelayInit(&delay_main,birthday);
		}
    }
}

void HardFault_Handler(){
	__ASM volatile( "BKPT #01	\n"	);
}
