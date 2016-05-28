/* Serial Port routines for the AVR mpu's.
*  These functions utilise the hardware UART registers
*
* By Adrian Gin (amg94@student.canterbury.ac.nz)
* Created: 16-06-06
* For AVR Core
*/

/* This is an example of how to use the routines*/
/* main prints out "I AM HERE" every 5 seconds, while echo'ing
 * any data received via the UARTs inputs*/
 
/* The inputs and outputs of the AVR need to be put through a MAX-232
 * tranciever to convert TTL -> RS232 and vice versa*/

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "../Pause/pause.h"
#include "../UART/Tx.h"
#include "hardUart.h"


/* For the rest of the baud rates at different CPU speeds,
 * See the datasheet under BaudRate generation*/	

int main(void)
{
	uartInit(BAUD19200, FAST);

	uint16_t temp;
	
	/*Enable interrupts*/
	sei();
	
	/*Enable receive complete interrupt*/
	UCSRB |=	(1<<RXCIE);
	
	/* performs an echo*/
	
	while(1)
	{
		//uartTxString("I AM HERE\r\n");
		
		pausems(500);	
	}
	
	
	return 0;
}



ISR(SIG_UART_RECV)
{
	//uartTxString("Rcvd its working");
	uartTx(UDR);
}



