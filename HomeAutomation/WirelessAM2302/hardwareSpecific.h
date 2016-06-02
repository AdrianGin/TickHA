
#ifndef _HARDWARE_SPECIFIC_H
#define _HARDWARE_SPECIFIC_H

#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/pgmspace.h>

#include "USARTn.h"
#include "SPI.h"


#include "AM2302.h"
#include "nRF24L01.h"

#include <util/delay_basic.h>

/*
#define SPI_DDR   (DDRB)
#define SPI_PORT  (PORTB)
#define SCK       (PB5)
#define MISO      (PB4)
#define MOSI      (PB3)
#define nSS       (PB2)*/

//It is important that this is very accurate or we get parity errors.
static inline void Delay_us(uint16_t us)
{
	 _delay_loop_2(us * (uint16_t)(F_CPU / 4e6) );
}



#endif
