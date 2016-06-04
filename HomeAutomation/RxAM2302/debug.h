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
#include "Log.h"


extern API::Log Log;

#ifndef NDEBUG

#define LOG_PRINT(level, string) Log.print_trace(level, 0, __func__, __LINE__); Log.print(level, (char*)string)
#define LOG_PRINT_DEC(level, string, num) Log.print_trace(level, __FILE__, __func__, __LINE__); Log.print_dec(level, (char*)string, num)
#define LOG_PRINT_HEX(level, string, num) Log.print_trace(level, __FILE__, __func__, __LINE__); Log.print_hex(level, (char*)string, num)
#define LOG_PRINT_HEXDUMP(level, string, ptr, num) Log.print_trace(level, __FILE__, __func__, __LINE__); Log.print_hexDump(level, (char*)string, ptr, num)

#else

#define LOG_PRINT(level, string) ((void)0)
#define LOG_PRINT_DEC(level, string, num) ((void)0)
#define LOG_PRINT_HEX(level, string, num) ((void)0)
#define LOG_PRINT_HEXDUMP(level, string, ptr, num) ((void)0)


#endif


#endif
