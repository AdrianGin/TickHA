

#include "USARTn.h"
#include "AM2302.h"

#include <stdlib.h>
#include <string.h>

#include "hardwareSpecific.h"

#include "debug.h"

#define DEBUG_LED_DDR (DDRB)
#define DEBUG_LED (PORTB)
#define DEBUG_LED_PIN (1<<0)
#define INPUT_SWITCH (PINC)
#define INPUT_SWITCH_PIN (1<<2)
#define INPUT_SWITCH_PIN2 (1<<3)



int main(void)
{

	char outputString[5];
	uint8_t receiveBuffer[96];

	memset(receiveBuffer, 0, 64);

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


	//uint8_t payload_len = 1;
	//nRF24L01_WriteRegister(&nRF24L01_Device, RX_PW_P1, &payload_len, 1);

	nRF24L01_Listen(&nRF24L01_Device, &nRF24L01_Device.local_address[0]);


	while(1)
	{
		nRF24L01_MainService(&nRF24L01_Device);
		if( nRF24L01_IsDataReady(&nRF24L01_Device) )
		{
			uint8_t bytesRxed = 0;
			bytesRxed = nRF24L01_GetData(&nRF24L01_Device, receiveBuffer);
			LOG_PRINT_DEC(LOG_INFO, "Rx Count: ", bytesRxed);
			if( bytesRxed == 1 )
			{
				receiveBuffer[1] = 0;
				LOG_PRINT(LOG_INFO, receiveBuffer);
			}
			else
			{
				LOG_PRINT_HEXDUMP(LOG_INFO, "Received: ", receiveBuffer, bytesRxed);
			}

			DEBUG_LED ^= (1<<DEBUG_LED_PIN);
			USARTn_TxString(&USART0, receiveBuffer);

		}

	}


	return 0;
}


ISR(USART_RX_vect)
{
	//uartTxString("Rcvd its working");
	USARTn_Tx(&USART0, UDR0);
}




