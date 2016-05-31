#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdlib.h>
#include "USARTn.h"

#ifndef NDEBUG

static inline void Debug_String(char* string)
{
	USARTn_TxString(&USART0, string);
}

void Debug_String16bit(char* string, uint16_t parameter);



#else

#define Debug_String(ignore) ((void)0)
#define Debug_String16bit(string, parameter) ((void)0)


#endif


#endif
