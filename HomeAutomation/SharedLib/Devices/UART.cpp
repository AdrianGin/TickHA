


#include "UART.h"

namespace Devices
{


void UART::tx(uint8_t* buffer, uint8_t nbytes )
{
	uint16_t i = 0;
	while( i++ < nbytes )
	{
		tx(*buffer++);
	}
}


void UART::tx(char* string)
{
	while( *string )
	{
		tx(*string++);
	}
}

void UART::tx_newline(void)
{
	tx('\r');
	tx('\n');
}




}
