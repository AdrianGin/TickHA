
#ifndef _DEVICES_GPIO_H
#define _DEVICES_GPIO_H

#include <stdint.h>


namespace Devices
{

class GPIO {

public:
	enum Direction {
		INPUT,
		OUTPUT,
	};

	enum LogicLevel {
		LOW,
		HIGH
	};

	LogicLevel currentlevel;
	Direction  currentDirection;

	//Initialises the GPIO with passed state
	virtual void Init( Direction state ) = 0;
	virtual void SetOutput( LogicLevel level) = 0;
	virtual uint8_t ReadInput() = 0;

};

}

#endif
