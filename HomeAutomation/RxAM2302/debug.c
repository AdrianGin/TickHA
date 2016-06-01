#include <stdint.h>
#include <avr/pgmspace.h>
#include "USARTn.h"
#include "debug.h"



#ifndef NDEBUG



uint8_t DebugLevel = LOG_INFO;



// Prints out the string
const char LOG_SEPARATOR[] PROGMEM = "::";
void log_print_trace(uint8_t loglevel, const char* file, const char* function, int line)
{

	if( DebugLevel <= loglevel )
	{
		USARTn_TxString(&USART0, (char*)function);
		USARTn_TxString_P(&USART0, LOG_SEPARATOR);

		char outputString[6];
		utoa(line, &outputString[0], 10);
		USARTn_TxString(&USART0, outputString);

		USARTn_TxString_P(&USART0, LOG_SEPARATOR);
	}
}


void log_print(uint8_t loglevel, char* string)
{
	if( DebugLevel <= loglevel )
	{
		USARTn_TxString(&USART0, string);
		USARTn_NewLine(&USART0);
	}
}

// Prints out the string followed by a number in decimal
void log_print_dec(uint8_t loglevel, char* string, uint16_t dec)
{

	if( DebugLevel <= loglevel )
	{
		char outputString[6];
		USARTn_TxString(&USART0, string);
		utoa(dec, &outputString[0], 10);
		USARTn_TxString(&USART0, outputString);
		USARTn_NewLine(&USART0);
	}
}

// Prints out the string followed by a number in hex
void log_print_hex(uint8_t loglevel, char* string, uint16_t hex)
{

	if( DebugLevel <= loglevel )
	{
		char outputString[6];
		USARTn_TxString(&USART0, string);
		utoa(hex, &outputString[0], 16);
		USARTn_TxString(&USART0, outputString);
		USARTn_NewLine(&USART0);
	}
}

// Prints out the string followed by a set of numbers in hex
void log_print_hexDump(uint8_t loglevel, char* string, uint8_t* dumpPtr, uint16_t n)
{

	if( DebugLevel <= loglevel )
	{
		char outputString[6];
		USARTn_TxString(&USART0, string);

		for( uint16_t i = 0; i < n; ++i)
		{
			utoa(dumpPtr[i], &outputString[0], 16);
			USARTn_TxString_P(&USART0, PSTR("0x"));
			USARTn_TxString(&USART0, outputString);
			USARTn_Tx(&USART0, ' ');

		}

		USARTn_NewLine(&USART0);
	}
}

#endif
