#ifndef _TSL2561_DEVICE
#define _TSL2561_DEVICE

#include <stdint.h>
#include <util/delay.h>

#include "I2C.h"

namespace Devices
{

class TSL2561
{
public:

	//Command register bit fields
	enum eCmdRegBits
	{
		ADDRESS = 0,
		BLOCK = 4,
		WORD,  //SMB W/R protocol.
		CLEAR, // Clear interrupt, write 1 to clear
		CMD, // Must be set
	};

	//Control register enums
	enum ePowerControl
	{
		POWER_DOWN = 0x00,
		POWER_UP = 0x03,
	};

	//Timing Register
	enum eTimingRegBits
	{
		INTEG = 0, //2 bits
		MANUAL = 3,
		GAIN = 4,
	};


	//Interrupt Register
	//Bit fields
	enum eIntRegBits
	{
		INT_PESIST = 0x00,
		INT_CONTROL = 0x04,
	};

	enum eIntControlSelect
	{
		INT_DISABLED 	 = 0x00,
		INT_LEVEL,
		INT_SMBCOMPLIANT,
		INT_TESTMODE,
	};

	//Interrupt Persist
	enum eInterruptPersist
	{
		INT_ADC_CYCLE = 0x00,
		INT_OUTSIDE_THRESHOLD,
		INT_2PERIODS,
		INT_3PERIODS,
		INT_4PERIODS,
		INT_5PERIODS,
		INT_6PERIODS,
		INT_7PERIODS,
		INT_8PERIODS,
		INT_9PERIODS,
		INT_10PERIODS,
		INT_11PERIODS,
		INT_12PERIODS,
		INT_13PERIODS,
		INT_14PERIODS,
		INT_15PERIODS,
	};

	//ID Register
	enum eIDRegisterBits
	{
		REVNO = 0x00,
		PARTNO = 0x04,
	};

	enum eRegisterAddresses
	{
		CONTROL = 0x00,
		TIMING,
		THRESHOLD_LOW_LOW,
		THRESHOLD_LOW_HIGH,
		THRESHOLD_HIGH_LOW,
		THRESHOLD_HIGH_HIGH,
		INTERRUPT,
		RESERVED1,
		CRC,
		RESERVED2,
		ID,
		RESERVED3,
		DATA0LOW,
		DATA0HIGH,
		DATA1LOW,
		DATA1HIGH,
	};


	enum eLightChannels
	{
		CH0,
		CH1
	};

	TSL2561(Devices::I2C& I2C, uint8_t i2cAddress) noexcept;

	//Sets up port as output high, ready for request.
	void Init();

	uint16_t GetRawLevel(uint8_t channel);
	uint16_t GetLuxLevel(void);


private:

	uint16_t CalculateLux(uint8_t iGain, uint8_t tInt, uint16_t ch0, uint16_t ch1);

	Devices::I2C& I2C;
	uint8_t i2cAddress;

	uint16_t ch0;
	uint16_t ch1;

};

}


#endif
