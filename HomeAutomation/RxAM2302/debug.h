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


#ifndef _DEBUG_H
#define _DEBUG_H


#include <stdlib.h>
#include "USARTn.h"


class DebugLog
{

public:
	enum
	{
		DEBUG = 0,
		INFO,
		WARN,
		ERR,
		FATAL,
	} ;


private:




};



void log_print_trace(uint8_t loglevel, const char* file, const char* function, int line);
void log_print(uint8_t loglevel, char* string);
// Prints out the string followed by a number in decimal
void log_print_dec(uint8_t loglevel, char* string, uint16_t dec);
// Prints out the string followed by a number in hex
void log_print_hex(uint8_t loglevel, char* string, uint16_t hex);
// Prints out the string followed by a set of numbers in hex
void log_print_hexDump(uint8_t loglevel, char* string, uint8_t* dumpPtr, uint16_t n);

#ifndef NDEBUG

#define LOG_PRINT(level, string) log_print_trace(level, 0, __func__, __LINE__); log_print(level, string)
#define LOG_PRINT_DEC(level, string, num) log_print_trace(level, __FILE__, __func__, __LINE__); log_print_dec(level, string, num)
#define LOG_PRINT_HEX(level, string, num) log_print_trace(level, __FILE__, __func__, __LINE__); log_print_hex(level, string, num)
#define LOG_PRINT_HEXDUMP(level, string, ptr, num) log_print_trace(level, __FILE__, __func__, __LINE__); log_print_hexDump(level, string, ptr, num)

#else

#define LOG_PRINT(level, string) ((void)0)
#define LOG_PRINT_DEC(level, string, num) ((void)0)
#define LOG_PRINT_HEX(level, string, num) ((void)0)
#define LOG_PRINT_HEXDUMP(level, string, ptr, num) ((void)0)


#endif



#endif
