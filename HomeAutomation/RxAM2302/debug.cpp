/*

Copyright (c) 2016 Adrian Gin (adrian.gin[at]gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


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

#else

void log_print_trace(uint8_t loglevel, const char* file, const char* function, int line) {}
void log_print(uint8_t loglevel, char* string) {}
// Prints out the string followed by a number in decimal
void log_print_dec(uint8_t loglevel, char* string, uint16_t dec) {}
// Prints out the string followed by a number in hex
void log_print_hex(uint8_t loglevel, char* string, uint16_t hex) {}
// Prints out the string followed by a set of numbers in hex
void log_print_hexDump(uint8_t loglevel, char* string, uint8_t* dumpPtr, uint16_t n) {}



#endif
