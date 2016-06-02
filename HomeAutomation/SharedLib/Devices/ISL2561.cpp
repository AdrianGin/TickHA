

#include <stdint.h>
#include "ISL2561.h"



//Sets up port as output high, ready for request.
void ISL2561_Init(ISL2561_t* dev)
{
	*dev->pin_out_reg |= (1 << (dev->pin_index));
	*dev->pin_dir_reg |= (1 << (dev->pin_index));
}


uint8_t ISL2561_WaitState(ISL2561_t* dev, uint8_t bitstate)
{
	for( uint8_t retry = 255; retry != 0; --retry )
	{
		if((*dev->pin_in_reg & (1<<dev->pin_index)) == (bitstate<<dev->pin_index) )
		{
			return 0;
		}
		ISL2561_Delay_us(10);
	}

	return 1;
}

//Sends a start signal of 1ms low, followed by a bus release,
//Waits for 100us and waits for pin to go low. (Should be after 180us)
uint8_t ISL2561_RequestData(ISL2561_t* dev)
{
	uint8_t connectionError = 0;

	ISL2561_Init(dev);

	*dev->pin_out_reg &= ~(1<<dev->pin_index);
	ISL2561_Delay_us(1000);
	*dev->pin_out_reg |= (1<<dev->pin_index);

	//Release bus
	*dev->pin_dir_reg &= ~(1 << (dev->pin_index));

	connectionError += ISL2561_WaitState(dev, 1);
	connectionError += ISL2561_WaitState(dev, 0);

	connectionError += ISL2561_WaitState(dev, 1);
	connectionError += ISL2561_WaitState(dev, 0);

	uint16_t humidity = 0;
	uint16_t temperature = 0;
	uint8_t  parity = 0;

	for( uint8_t i = 0; i < 16; ++i)
	{
		humidity = humidity << 1;
		humidity |= ISL2561_GetBit(dev);

	}

	for( uint8_t i = 0; i < 16; ++i)
	{
		temperature = temperature << 1;
		temperature |= ISL2561_GetBit(dev);

	}

	for( uint8_t i = 0; i < 8; ++i)
	{
		parity = parity << 1;
		parity |= ISL2561_GetBit(dev);
	}

	uint8_t calculatedParity = ((humidity & 0xFF00) >> 8) + (humidity & 0xFF) + \
							   ((temperature & 0xFF00) >> 8) + (temperature & 0xFF);



	if( connectionError )
	{
		return ISL2561_ERR_CONNECTION;
	}

	if( parity == calculatedParity )
	{

		dev->humidity = humidity;
		dev->temperature = temperature;
		return 0;
	}
	else
	{
		return ISL2561_ERR_PARITY;
	}
}





uint8_t ISL2561_GetBit(ISL2561_t* dev)
{

	uint8_t bitval;
	ISL2561_WaitState(dev, 0);
	ISL2561_WaitState(dev, 1);
	ISL2561_Delay_us(30);

	bitval = (*dev->pin_in_reg & (1 << (dev->pin_index)) );

	return bitval == 0 ? 0 : 1;

}


uint16_t ISL2561_GetRawTemperature(ISL2561_t* dev)
{
	return dev->temperature;
}

uint16_t ISL2561_GetRawHumidity(ISL2561_t* dev)
{
	return dev->humidity;
}


int8_t ISL2561_GetTemperature(ISL2561_t* dev)
{
	if( dev->temperature & ISL2561_TEMPSIGN_BIT )
	{
		return -(dev->temperature / 10);
	}

	return (dev->temperature / 10);
}

int8_t ISL2561_GetHumidity(ISL2561_t* dev)
{
	return dev->humidity / 10;
}

