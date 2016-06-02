

#include <avr/pgmspace.h>

#include "USARTn.h"
#include "AM2302.h"

#include <stdlib.h>
#include <string.h>

#include "hardwareSpecific.h"

#define DEBUG_LED_DDR (DDRB)
#define DEBUG_LED (PORTB)
#define DEBUG_LED_PIN (1<<0)
#define INPUT_SWITCH (PINC)
#define INPUT_SWITCH_PIN (1<<2)
#define INPUT_SWITCH_PIN2 (1<<3)


volatile uint8_t rxChar = 0;
volatile uint8_t rxFlag = 0;

int main(void)
{

	char outputString[5];


	clock_prescale_set(clock_div_1);
	DEBUG_LED_DDR |= (DEBUG_LED_PIN);				//Set B1 to output for dbg LED
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

	uint8_t dest_Address[] = {0xE8,0xE8,0xE8,0xE8,0xE8};

	while(1)
	{

		nRF24L01_MainService(&nRF24L01_Device);
		//Perform a double blink to indicate it is working

		uint8_t err = AM2302_RequestData(&AM2302_Device);
		if( rxFlag )
		{
			nRF24L01_Transmit(&nRF24L01_Device, &dest_Address[0], (uint8_t*)&rxChar, 1);
			nRF24L01_TransferSync(&nRF24L01_Device);
			rxFlag = 0;
		}

		DEBUG_LED &= ~(1<<DEBUG_LED_PIN);
		_delay_ms(1000);
		DEBUG_LED |= (1<<DEBUG_LED_PIN);
		_delay_ms(1000);

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
				{
					char humidityStr[25] = "The humidity is: ";
					char tempStr[25] = "The temperature is: ";

					USARTn_TxString(&USART0, (humidityStr));
					itoa( AM2302_GetHumidity(&AM2302_Device), &outputString[0], 10);
					USARTn_TxString(&USART0, outputString);
					USARTn_NewLine(&USART0);

					strcat(humidityStr, outputString);
					strncat(humidityStr, "\n", 2);
					nRF24L01_Transmit(&nRF24L01_Device, &dest_Address[0], (uint8_t*)humidityStr, strlen(humidityStr)+1);
					nRF24L01_TransferSync(&nRF24L01_Device);

					USARTn_TxString(&USART0, (tempStr));
					itoa( AM2302_GetTemperature(&AM2302_Device), &outputString[0], 10);
					USARTn_TxString(&USART0, outputString);
					USARTn_NewLine(&USART0);
					USARTn_NewLine(&USART0);
					USARTn_NewLine(&USART0);

					strcat(tempStr, outputString);
					strncat(tempStr, "\n", 2);
					nRF24L01_Transmit(&nRF24L01_Device, &dest_Address[0], (uint8_t*)tempStr, strlen(tempStr)+1);
					nRF24L01_TransferSync(&nRF24L01_Device);

					break;
				}
		}
	}


	return 0;
}



ISR(USART_RX_vect)
{
	rxChar = UDR0;
	rxFlag = 1;
	//uartTxString("Rcvd its working");
	USARTn_Tx(&USART0, rxChar);
}




