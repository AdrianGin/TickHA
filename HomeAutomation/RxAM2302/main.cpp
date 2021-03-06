
#include <AVRUSARTn.h>
#include "AVRGPIO.h"
#include "AVRSPI.h"
#include "AM2302.h"
#include "nRF24L01.h"
#include <stdlib.h>
#include <string.h>

#include "hardwareSpecific.h"

#include "hw_config.h"
#include "Log.h"

AVR::GPIO DebugLED = AVR::GPIO(DDRB, PORTB, PINB, PB0);

AVR::GPIO SCK = AVR::GPIO(DDRB, PORTB, PINB, PB5);
AVR::GPIO MISO = AVR::GPIO(DDRB, PORTB, PINB, PB4);
AVR::GPIO MOSI = AVR::GPIO(DDRB, PORTB, PINB, PB3);
AVR::GPIO nSS = AVR::GPIO(DDRB, PORTB, PINB, PB2);


AVR::SPI SPI1 = AVR::SPI(SCK, MISO, MOSI, nSS);

AVR::GPIO nRF24CE = AVR::GPIO(DDRD, PORTD, PIND, PD7);
AVR::GPIO nRF24CSN = AVR::GPIO(DDRD, PORTD, PIND, PD6);
AVR::GPIO nRF24IRQ = AVR::GPIO(DDRD, PORTD, PIND, PD5);

Devices::nRF24L01 WirelessDev = Devices::nRF24L01(1, SPI1, nRF24CE, nRF24CSN, nRF24IRQ);



int main(void)
{

	char outputString[5];
	uint8_t receiveBuffer[96];

	memset(receiveBuffer, 0, 64);

	clock_prescale_set(clock_div_1);

	DebugLED.Init( Devices::GPIO::OUTPUT );
	DebugLED.SetOutput( Devices::GPIO::HIGH );

	USART0.Init(BAUD19200);
	/*Enable interrupts*/
	sei();

	/*Enable receive complete interrupt*/

	_delay_ms(100);

	SPI1.Init();
	WirelessDev.Init();
	WirelessDev.SetAckState(1);

	//uint8_t payload_len = 1;
	//nRF24L01_WriteRegister(&nRF24L01_Device, RX_PW_P1, &payload_len, 1);

	uint8_t my_Address[] = {0xE8,0xE8,0xE8,0xE8,0xE8};

	WirelessDev.Listen(&my_Address[0]);

	uint8_t packetCount = 0;

	while(1)
	{
		WirelessDev.MainService();
		if( WirelessDev.IsDataReady() )
		{
			uint8_t bytesRxed = 0;
			bytesRxed = WirelessDev.GetData(receiveBuffer);
			LOG_PRINT_DEC(API::Log::INFO, "Rx Count: ", bytesRxed);
			if( bytesRxed == 1 )
			{
				receiveBuffer[1] = 0;
				LOG_PRINT(API::Log::INFO, (char*)receiveBuffer);
			}
			else
			{
				LOG_PRINT_HEXDUMP(API::Log::INFO, "Received: ", receiveBuffer, bytesRxed);
			}

			USART0.tx( (char*) receiveBuffer);
			packetCount++;
		}

		if( packetCount >= 2 )
		{
			LOG_PRINT_DEC(API::Log::INFO, "CurrentLED: ", DebugLED.currentlevel );
			DebugLED.SetOutput( (Devices::GPIO::LogicLevel)(DebugLED.currentlevel ^ 1) );
			packetCount = 0;
		}

	}


	return 0;
}


ISR(USART_RX_vect)
{
	//uartTxString("Rcvd its working");
	USART0.tx(UDR0);
}




