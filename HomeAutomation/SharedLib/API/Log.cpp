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

#include "UART.h"
#include "Log.h"

extern uint8_t DebugLevel;

namespace API
{


// Prints out the string
const char LOG_SEPARATOR[] PROGMEM = "::";

Log::Log(Devices::UART& term) noexcept : term(term)
{
}


void Log::print_trace(uint8_t loglevel, char* file, char* function, int line)
{

	if( DebugLevel <= loglevel )
	{
		term.tx(function);
		term.tx(LOG_SEPARATOR);

		char outputString[6];
		utoa(line, &outputString[0], 10);
		term.tx(outputString);

		term.tx(LOG_SEPARATOR);
	}
}


void Log::print(uint8_t loglevel, char* string)
{
	if( DebugLevel <= loglevel )
	{
		term.tx(string);
		term.tx_newline();
	}
}

// Prints out the string followed by a number in decimal
void Log::print_dec(uint8_t loglevel, char* string, uint16_t dec)
{

	if( DebugLevel <= loglevel )
	{
		char outputString[6];
		term.tx(string);
		utoa(dec, &outputString[0], 10);
		term.tx(outputString);
		term.tx_newline();
	}
}

// Prints out the string followed by a number in hex
void Log::print_hex(uint8_t loglevel, char* string, uint16_t hex)
{

	if( DebugLevel <= loglevel )
	{
		char outputString[6];
		term.tx(string);
		utoa(hex, &outputString[0], 16);
		term.tx(outputString);
		term.tx_newline();
	}
}

// Prints out the string followed by a set of numbers in hex
void Log::print_hexDump(uint8_t loglevel, char* string, uint8_t* dumpPtr, uint16_t n)
{

	if( DebugLevel <= loglevel )
	{
		char outputString[6];
		term.tx(string);

		for( uint16_t i = 0; i < n; ++i)
		{
			utoa(dumpPtr[i], &outputString[0], 16);
			term.tx(PSTR("0x"));
			term.tx(outputString);
			term.tx(' ');

		}

		term.tx_newline();
	}
}


}


