
#ifndef _AVR_GPIO_H
#define _AVR_GPIO_H


#include "AVRGPIO.h"
#include "GPIO.h"
#include <stdint.h>

using Devices::GPIO;

namespace AVR
{

class GPIO : public Devices::GPIO
{
public:
	GPIO(volatile uint8_t& DDR, volatile uint8_t& PORT, volatile uint8_t& PIN, uint8_t pn) noexcept;

	void Init( Direction state );
	void SetOutput( LogicLevel level);
	uint8_t ReadInput();

private:
	volatile uint8_t &DDR;
	volatile uint8_t &PORT;
	volatile uint8_t &PIN;

	const uint8_t pinNumber;

};


}
#endif
