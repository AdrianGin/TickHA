#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include <avr/pgmspace.h>
#include <USARTn/USARTn.h>

#define DEBUG_LED (PORTD)
#define DEBUG_LED_PIN (1<<6)
#define INPUT_SWITCH (PINC)
#define INPUT_SWITCH_PIN (1<<2)
#define INPUT_SWITCH_PIN2 (1<<3)

int main(void)
{



	clock_prescale_set(clock_div_1);
	DDRD |= (DEBUG_LED_PIN);				//Set B1 to output for dbg LED
	DEBUG_LED &= ~DEBUG_LED_PIN;			//Turn off dbg LED

	USARTn_Init(&USART0, BAUD19200, FAST);
	/*Enable interrupts*/
	sei();

	/*Enable receive complete interrupt*/
	*USART0.UCSRnB |= (1<<RXCIEn);

	while(1)
	{
		//Perform a double blink to indicate it is working
		DEBUG_LED |= (DEBUG_LED_PIN);
		_delay_ms(500);
		DEBUG_LED &= ~(DEBUG_LED_PIN);
		_delay_ms(500);
		DEBUG_LED |= (DEBUG_LED_PIN);
		_delay_ms(500);
		DEBUG_LED &= ~(DEBUG_LED_PIN);
		//======

		USARTn_TxString(&USART0, ("The temperature is: "));


	}


	return 0;
}


ISR(USART_RX_vect)
{
	//uartTxString("Rcvd its working");
	USARTn_Tx(&USART0, UDR0);
}




