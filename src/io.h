#ifndef __IO_H
#define __IO_H

#include "stm32f0xx_gpio.h"
#include "main.h"

void IO_Init();

#define IODESC( p, m, t, pp ) { p, m, t ,pp, 0 }
#define IODESCAF( p, t, pp, af ) { p, GPIO_Mode_AF, t ,pp, af }
#define IODESC_END() { NULL, 0, 0, 0, 0 }

#define MEPIN( _port, _pin ) _port, _pin

#define MEPIN_SET_( _port, _pin ) _port->BSRR = (1<<_pin)
#define MEPIN_SET( _p_ ) MEPIN_SET_( _p_ )

#define MEPIN_CLR_( _port, _pin ) _port->BRR = (1<<_pin)
#define MEPIN_CLR( _p_ ) MEPIN_CLR_( _p_ )

#define MEPIN_GET_( _port, _pin ) (_port->IDR & (1<<_pin))
#define MEPIN_GET( _p_ ) MEPIN_GET_( _p_ )

#define MEPIN_SWAP_( _port, _pin ) _port->BSRR = MEPIN_GET_(_port, _pin) ? ( 1<<(_pin+16) ) : 1<<(_pin)
#define MEPIN_SWAP( _p_ ) MEPIN_SWAP_( _p_ )

#define MEPIN_GET_PIN_MASK_( _port, _pin ) (1<<_pin)
#define MEPIN_GET_PIN_MASK( _p_ ) MEPIN_GET_PIN_MASK_( _p_ )

#define MEPIN_GET_PIN_GPIO_( _port, _pin ) (_port)
#define MEPIN_GET_PIN_GPIO( _p_ ) MEPIN_GET_PIN_GPIO_( _p_ )



// /////////////////////////////////////////////////////////////////////


#define PIN_LED0 			MEPIN( GPIOA, 5 )

#define PIN_USART2TX        MEPIN( GPIOA, 2 )
#define PIN_USART2RX        MEPIN( GPIOA, 3 )

#define PIN_ADC1            MEPIN( GPIOC, 0 )
#define PIN_ADC2            MEPIN( GPIOC, 1 )

#define PIN_SPI_CS          MEPIN( GPIOB, 10 )

#define PIN_USER_BUTTON     MEPIN( GPIOC, 13 )

#ifdef __USE_MCO__
    #define PIN_MCO             MEPIN( GPIOA, 8 )
#endif

#endif
