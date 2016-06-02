

#include <stdint.h>
#include "AM2302.h"
#include "GPIO.h"
#include "debug.h"



namespace Devices
{

AM2302::AM2302(GPIO& sda, void(*delay_us)(uint16_t)) noexcept : sda(sda), delay_us(delay_us)
{
	humidity = 0;
	temperature = 0;
}

void AM2302::Init(void)
{
	sda.Init( Devices::GPIO::OUTPUT );
	sda.SetOutput( Devices::GPIO::HIGH );
}


uint16_t AM2302::GetRawTemperature()
{
	return temperature;
}

uint16_t AM2302::GetRawHumidity()
{
	return humidity;
}


int8_t AM2302::GetTemperature()
{
	if( temperature & TEMPSIGN_BIT )
	{
		return -(temperature / 10);
	}

	return (temperature / 10);
}

int8_t AM2302::GetHumidity()
{
	return humidity / 10;
}

//Sends a start signal of 1ms low, followed by a bus release,
//Waits for 100us and waits for pin to go low. (Should be after 180us)
uint8_t AM2302::RequestData(void)
{
	uint8_t connectionError = 0;

	Init();

	sda.SetOutput( Devices::GPIO::LOW );
	delay_us(1000);
	sda.SetOutput( Devices::GPIO::HIGH );

	//Release bus
	sda.Init( Devices::GPIO::INPUT );

	connectionError += WaitState(Devices::GPIO::HIGH);
	connectionError += WaitState(Devices::GPIO::LOW);

	connectionError += WaitState(Devices::GPIO::HIGH);
	connectionError += WaitState(Devices::GPIO::LOW);

	uint16_t humidity = 0;
	uint16_t temperature = 0;
	uint8_t  parity = 0;

	for( uint8_t i = 0; i < 16; ++i)
	{
		humidity = humidity << 1;
		humidity |= GetBit();

	}

	for( uint8_t i = 0; i < 16; ++i)
	{
		temperature = temperature << 1;
		temperature |= GetBit();

	}

	for( uint8_t i = 0; i < 8; ++i)
	{
		parity = parity << 1;
		parity |= GetBit();
	}

	uint8_t calculatedParity = ((humidity & 0xFF00) >> 8) + (humidity & 0xFF) + \
							   ((temperature & 0xFF00) >> 8) + (temperature & 0xFF);



	if( connectionError )
	{
		return ERR_CONNECTION;
	}

	if( parity == calculatedParity )
	{

		this->humidity = humidity;
		this->temperature = temperature;
		return 0;
	}
	else
	{

		LOG_PRINT_DEC(LOG_INFO, (char*)"Humidity : ", humidity);
		LOG_PRINT_DEC(LOG_INFO, (char*)"Temperature: ", temperature);

		LOG_PRINT_DEC(LOG_ERR,  (char*)"Parity Error Rx: ", parity);
		LOG_PRINT_DEC(LOG_ERR,  (char*)"Expected: ", calculatedParity);

		return ERR_PARITY;
	}
}




uint8_t AM2302::WaitState(Devices::GPIO::LogicLevel level)
{
	for( uint8_t retry = 255; retry != 0; --retry )
	{
		if( sda.ReadInput() == level )
		{
			return 0;
		}
		delay_us(10);
	}

	return 1;
}





uint8_t AM2302::GetBit()
{
	WaitState( Devices::GPIO::LOW );
	WaitState( Devices::GPIO::HIGH );
	delay_us(30);
	return sda.ReadInput();
}


}




