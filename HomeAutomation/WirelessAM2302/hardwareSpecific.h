
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

extern nRF24L01_t nRF24L01_Device;
extern AM2302_t AM2302_Device;


#define SPI_DDR   (DDRB)
#define SPI_PORT  (PORTB)
#define SCK       (PB5)
#define MISO      (PB4)
#define MOSI      (PB3)
#define nSS       (PB2)


void nRF24_Init(void);
void nRF24_SetCS(uint8_t state);
void nRF24_SetCE(uint8_t state);
uint8_t nRF24_GetIRQ(void);

void nRF24_IRQCallback(nRF24L01_t* dev);


#endif
