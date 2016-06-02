/*

Copyright (c) 2016 Adrian Gin (adrian.gin[at]gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


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
