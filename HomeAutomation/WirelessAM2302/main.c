
<<<<<<< HEAD
#include <avr/pgmspace.h>
=======

>>>>>>> b6f32ac850cf5a1a0fbffd05dcf4e1a44df2ec6e
#include "USARTn.h"
#include "AM2302.h"

#include <stdlib.h>

#include "hardwareSpecific.h"

#define DEBUG_LED_DDR (DDRB)
#define DEBUG_LED (PORTB)
#define DEBUG_LED_PIN (1<<0)
#define INPUT_SWITCH (PINC)
#define INPUT_SWITCH_PIN (1<<2)
#define INPUT_SWITCH_PIN2 (1<<3)



int main(void)
{

	char outputString[5];


	clock_prescale_set(clock_div_1);
	DEBUG_LED_DDR |= (DEBUG_LED_PIN);		//Set B1 to output for dbg LED
	DEBUG_LED &= ~DEBUG_LED_PIN;			//Turn off dbg LED

	USARTn_Init(&USART0, BAUD19200, FAST);
	/*Enable interrupts*/
	sei();

	/*Enable receive complete interrupt*/
	*USART0.UCSRnB |= (1<<RXCIEn);

	_delay_ms(100);

	SPI_Init();
	nRF24L01_Init(&nRF24L01_Device);
	nRF24L01_SetAckState(&nRF24L01_Device , 1);

	uint8_t payload_len = 1;
	nRF24L01_WriteRegister(&nRF24L01_Device, RX_PW_P1, &payload_len, 1);

	uint8_t dest_Address[] = {0xE8,0xE8,0xE8,0xE8,0xE8};

	while(1)
	{

		nRF24L01_Transmit(&nRF24L01_Device, &dest_Address[0], "Hello", 1 );
		nRF24L01_TransferSync(&nRF24L01_Device);
		//Perform a double blink to indicate it is working
		DEBUG_LED |= (DEBUG_LED_PIN);
		_delay_ms(1000);
		DEBUG_LED &= ~(DEBUG_LED_PIN);
		_delay_ms(1000);

		uint8_t err = AM2302_RequestData(&AM2302_Device);

		switch( err )
		{
			case AM2302_ERR_PARITY:
				USARTn_TxString(&USART0, ("Parity Error"));
				USARTn_NewLine(&USART0);
				break;

			case AM2302_ERR_CONNECTION:
				USARTn_TxString(&USART0, ("Connection Error"));
				USARTn_NewLine(&USART0);
				break;

			default:
				USARTn_TxString(&USART0, ("The humidity is: "));
				itoa( AM2302_GetHumidity(&AM2302_Device), &outputString[0], 10);
				USARTn_TxString(&USART0, outputString);
				USARTn_NewLine(&USART0);

				USARTn_TxString(&USART0, ("The temperature is: "));
				itoa( AM2302_GetTemperature(&AM2302_Device), &outputString[0], 10);
				USARTn_TxString(&USART0, outputString);
				USARTn_NewLine(&USART0);
				USARTn_NewLine(&USART0);
				USARTn_NewLine(&USART0);
				break;
		}
	}


	return 0;
}


ISR(USART_RX_vect)
{
	//uartTxString("Rcvd its working");
	USARTn_Tx(&USART0, UDR0);
}




