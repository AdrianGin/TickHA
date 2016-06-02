#ifndef _AM2302_DEVICE
#define _AM2302_DEVICE

#include <stdint.h>
#include "GPIO.h"

namespace Devices
{

class AM2302
{
public:
	//Error codes
	enum
	{
		ERR_PARITY  	=   1,
		ERR_CONNECTION 	=   2,
	};

	enum {
		TEMPSIGN_BIT = (0x8000)
	};

	uint16_t humidity;
	uint16_t temperature;


	//Need to provide the GPIO port and a delay us function.
	AM2302(GPIO& sda, void(*delay_us)(uint16_t)) noexcept;

	uint8_t RequestData(void);
	int8_t GetTemperature();
	int8_t GetHumidity();

	uint16_t GetRawTemperature();
	uint16_t GetRawHumidity();

private:
	GPIO& sda;
	void (*delay_us)(uint16_t us);

	//Sets up port as output high, ready for request.
	void Init(void);
	uint8_t WaitState(Devices::GPIO::LogicLevel level);
	uint8_t GetBit();

};





}


#endif
