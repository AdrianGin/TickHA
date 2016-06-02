

#include <avr/pgmspace.h>
#include "AVRGPIO.h"
#include "AVRSPI.h"

#include "USARTn.h"
#include "AM2302.h"
#include "nRF24L01.h"

#include <stdlib.h>
#include <string.h>

#include "hardwareSpecific.h"



#define DEBUG_LED_DDR (DDRB)
#define DEBUG_LED (PORTB)
#define DEBUG_LED_PIN (1<<0)
#define INPUT_SWITCH (PINC)
#define INPUT_SWITCH_PIN (1<<2)
#define INPUT_SWITCH_PIN2 (1<<3)

AVR::GPIO DebugLED = AVR::GPIO(DDRB, PORTB, PINB, 0);

AVR::GPIO nRF24CE = AVR::GPIO(DDRD, PORTD, PIND, 7);
AVR::GPIO nRF24CSN = AVR::GPIO(DDRD, PORTD, PIND, 6);
AVR::GPIO nRF24IRQ = AVR::GPIO(DDRD, PORTD, PIND, 5);

AVR::GPIO AM2302IO = AVR::GPIO(DDRC, PORTC, PINC, PC5);

AVR::GPIO SCK = AVR::GPIO(DDRB, PORTB, PINB, PB5);
AVR::GPIO MISO = AVR::GPIO(DDRB, PORTB, PINB, PB4);
AVR::GPIO MOSI = AVR::GPIO(DDRB, PORTB, PINB, PB3);
AVR::GPIO nSS = AVR::GPIO(DDRB, PORTB, PINB, PB2);


AVR::SPI SPI1 = AVR::SPI(SCK, MISO, MOSI, nSS);


Devices::AM2302   ThermSensor = Devices::AM2302(AM2302IO, Delay_us);
Devices::nRF24L01 WirelessDev = Devices::nRF24L01(1, SPI1, nRF24CE, nRF24CSN, nRF24IRQ);


volatile uint8_t rxChar = 0;
volatile uint8_t rxFlag = 0;

int main(void)
{
	char outputString[5];

	clock_prescale_set(clock_div_1);
	DebugLED.Init( Devices::GPIO::OUTPUT );
	DebugLED.SetOutput( Devices::GPIO::HIGH );

	USARTn_Init(&USART0, BAUD19200, FAST);
	/*Enable interrupts*/
	sei();

	/*Enable receive complete interrupt*/
	*USART0.UCSRnB |= (1<<RXCIEn);

	_delay_ms(100);

	SPI1.Init();

	WirelessDev.Init();
	WirelessDev.SetAckState(1);

	uint8_t dest_Address[] = {0xE8,0xE8,0xE8,0xE8,0xE8};

	while(1)
	{
		WirelessDev.MainService();
		//Perform a double blink to indicate it is working

		uint8_t err = ThermSensor.RequestData();
		if( rxFlag )
		{
			WirelessDev.Transmit(&dest_Address[0], (uint8_t*)&rxChar, 1);
			WirelessDev.TransferSync();
			rxFlag = 0;
		}

		DebugLED.SetOutput( Devices::GPIO::HIGH );
		_delay_ms(1000);
		DebugLED.SetOutput( Devices::GPIO::LOW);
		_delay_ms(1000);

		switch( err )
		{
			case Devices::AM2302::ERR_PARITY:
				USARTn_TxString(&USART0, ("Parity Error"));
				USARTn_NewLine(&USART0);
				break;

			case Devices::AM2302::ERR_CONNECTION:
				USARTn_TxString(&USART0, ("Connection Error"));
				USARTn_NewLine(&USART0);
				break;

			default:
				{
					char humidityStr[25] = "The humidity is: ";
					char tempStr[25] = "The temperature is: ";

					USARTn_TxString(&USART0, (humidityStr));
					itoa( ThermSensor.GetHumidity(), &outputString[0], 10);
					USARTn_TxString(&USART0, outputString);
					USARTn_NewLine(&USART0);

					strcat(humidityStr, outputString);
					strncat(humidityStr, "\n", 2);
					WirelessDev.Transmit(&dest_Address[0], (uint8_t*)humidityStr, strlen(humidityStr)+1);
					WirelessDev.TransferSync();

					USARTn_TxString(&USART0, (tempStr));
					itoa( ThermSensor.GetTemperature(), &outputString[0], 10);
					USARTn_TxString(&USART0, outputString);
					USARTn_NewLine(&USART0);
					USARTn_NewLine(&USART0);
					USARTn_NewLine(&USART0);

					strcat(tempStr, outputString);
					strncat(tempStr, "\n", 2);
					WirelessDev.Transmit(&dest_Address[0], (uint8_t*)tempStr, strlen(tempStr)+1);
					WirelessDev.TransferSync();

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




