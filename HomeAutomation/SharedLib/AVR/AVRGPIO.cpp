



#include "AVRGPIO.h"
#include "GPIO.h"

using Devices::GPIO;

namespace AVR
{

GPIO::GPIO(volatile uint8_t& DDR, volatile uint8_t& PORT, volatile uint8_t& PIN, uint8_t pn) noexcept:
		DDR(DDR), PORT(PORT), PIN(PIN), pinNumber(pn)
{

}

void GPIO::Init( Direction dir )
{
	if( dir == INPUT )
	{
		DDR &= ~(1 << pinNumber);
	}
	else
	{
		DDR |= (1 << pinNumber);
	}
	currentDirection = dir;
}

void GPIO::SetOutput( LogicLevel level)
{
	if( level == currentlevel )
	{
		return;
	}

	if( level == LOW )
	{
		PORT &= ~(1 << pinNumber);
	}
	else
	{
		PORT |= (1 << pinNumber);
	}
	currentlevel = level;
}


uint8_t GPIO::ReadInput(void)
{

	return (PIN & (1<<pinNumber)) ? 1 : 0;
}

}
