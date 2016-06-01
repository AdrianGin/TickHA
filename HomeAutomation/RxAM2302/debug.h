#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdlib.h>
#include "USARTn.h"

enum
{
	LOG_DEBUG = 0,
	LOG_INFO,
	LOG_WARN,
	LOG_ERR,
	LOG_FATAL,
} ;

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

#define LOG_PRINT(level, string) (void(0))
#define LOG_PRINT_DEC(level, string, num) (void(0))
#define LOG_PRINT_HEX(level, string, num) (void(0))
#define LOG_PRINT_HEXDUMP(level, string, ptr, num) (void(0))

#endif


#endif
