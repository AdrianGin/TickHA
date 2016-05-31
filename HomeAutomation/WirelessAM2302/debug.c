#include "debug.h"


#ifndef NDEBUG

void Debug_String16bit(char* string, uint16_t parameter)
{
	char outputString[6];
	USARTn_TxString(&USART0, string);
	utoa(parameter, &outputString[0], 10);
	USARTn_TxString(&USART0, outputString);

	USARTn_NewLine(&USART0);
}

#endif
