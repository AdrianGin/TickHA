


#include "UART.h"

namespace Devices
{

void UART::tx_newline(void)
{
	tx('\r');
	tx('\n');
}


}
