

#include <stdint.h>
#include "AM2302.h"

#include "USARTn/USARTn.h"

//Sets up port as output high, ready for request.
void AM2302_Init(AM2302_t* dev)
{
	*dev->pin_out_reg |= (1 << (dev->pin_index));
	*dev->pin_dir_reg |= (1 << (dev->pin_index));
}


void AM2302_WaitState(AM2302_t* dev, uint8_t bitstate)
{
	while( (*dev->pin_in_reg & (1<<dev->pin_index)) != (bitstate<<dev->pin_index) );
}

//Sends a start signal of 1ms low, followed by a bus release,
//Waits for 100us and waits for pin to go low. (Should be after 180us)
uint8_t AM2302_RequestData(AM2302_t* dev)
{
	AM2302_Init(dev);

	*dev->pin_out_reg &= ~(1<<dev->pin_index);
	AM2302_Delay_us(1000);
	*dev->pin_out_reg |= (1<<dev->pin_index);

	//Release bus
	*dev->pin_dir_reg &= ~(1 << (dev->pin_index));

	AM2302_WaitState(dev, 1);
	AM2302_WaitState(dev, 0);

	AM2302_WaitState(dev, 1);
	AM2302_WaitState(dev, 0);

	uint16_t humidity = 0;
	uint16_t temperature = 0;
	uint8_t  parity = 0;

	for( uint8_t i = 0; i < 16; ++i)
	{
		humidity = humidity << 1;
		humidity |= AM2302_GetBit(dev);

	}

	for( uint8_t i = 0; i < 16; ++i)
	{
		temperature = temperature << 1;
		temperature |= AM2302_GetBit(dev);

	}

	for( uint8_t i = 0; i < 8; ++i)
	{
		parity = parity << 1;
		parity |= AM2302_GetBit(dev);
	}

	uint8_t calculatedParity = ((humidity & 0xFF00) >> 8) + (humidity & 0xFF) + \
							   ((temperature & 0xFF00) >> 8) + (temperature & 0xFF);



	if( parity == calculatedParity )
	{

		dev->humidity = humidity;
		dev->temperature = temperature;
		return 0;
	}

	return 1;

}





uint8_t AM2302_GetBit(AM2302_t* dev)
{

	uint8_t bitval;
	AM2302_WaitState(dev, 0);
	AM2302_WaitState(dev, 1);
	AM2302_Delay_us(30);

	bitval = (*dev->pin_in_reg & (1 << (dev->pin_index)) );

	return bitval == 0 ? 0 : 1;

}


uint16_t AM2302_GetTemperature(AM2302_t* dev)
{
	return dev->temperature;
}

uint16_t AM2302_GetHumidity(AM2302_t* dev)
{
	return dev->humidity;
}




