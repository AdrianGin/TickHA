
#include "hardwareSpecific.h"



nRF24L01_t nRF24L01_Device = {

		.rfChannel = 1,
		.local_address = {0xE8,0xE8,0xE8,0xE8,0xE8},

		.IOInit   = &nRF24_Init,
		.SPIRead  = &SPI_RxByte,
		.SPIWrite = &SPI_TxByte,

		.ChipSelect 	= &nRF24_SetCS,
		.ChipEnable = &nRF24_SetCE,
		.GetIRQState    = &nRF24_GetIRQ,
		.IRQCallback 	= &nRF24_IRQCallback,

};


AM2302_t AM2302_Device = {
	.pin_dir_reg = &DDRC,
	.pin_out_reg = &PORTC,
	.pin_in_reg  = &PINC,

	.pin_index  = PC5,
};




#define nRF24_CE_DDR   DDRD
#define nRF24_CE_PORT  PORTD
#define nRF24_CE_PIN   7

#define nRF24_CSN_DDR  DDRD
#define nRF24_CSN_PORT PORTD
#define nRF24_CSN_PIN  6

#define nRF24_IRQ_DDR  DDRD
#define nRF24_IRQ_PORT PORTD
#define nRF24_IRQ_IN   PIND
#define nRF24_IRQ_PIN  5

void nRF24_Init(void)
{

	nRF24_CE_DDR |= (1 << nRF24_CE_PIN);
	nRF24_CSN_DDR |= (1 << nRF24_CSN_PIN);
	nRF24_IRQ_DDR &= ~(1 << nRF24_IRQ_PIN);

	nRF24_CE_PORT  |= (1<<nRF24_CE_PIN);
	nRF24_CSN_PORT |= (1<<nRF24_CSN_PIN);
	nRF24_IRQ_PORT |= (1 << nRF24_IRQ_PIN);



	SPI_Init();

}

void nRF24_SetCS(uint8_t state)
{
	if( state )
	{
		nRF24_CSN_PORT |= (1 << nRF24_CSN_PIN);
	}
	else
	{
		nRF24_CSN_PORT &= ~(1 << nRF24_CSN_PIN);
	}
}


void nRF24_SetCE(uint8_t state)
{
	if( state )
	{
		nRF24_CE_PORT |= (1 << nRF24_CE_PIN);
	}
	else
	{
		nRF24_CE_PORT &= ~(1 << nRF24_CE_PIN);
	}
}


uint8_t nRF24_GetIRQ(void)
{
	return (nRF24_IRQ_IN & (1<<nRF24_IRQ_PIN)) ? 1 : 0;
}

void nRF24_IRQCallback(nRF24L01_t* dev)
{

}





