#ifndef _BMP180_DEVICE
#define _BMP180_DEVICE

#include <stdint.h>
#include <util/delay.h>

#include "I2C.h"

namespace Devices
{

class BMP180
{
public:

	enum
	{
		I2CAddress = 0xEE
	};

	enum eSoftReset
	{
		RESET = 0xB6 //Write this to SOFT_RESET to perform POR
	};

	enum eOversamplingMode
	{
		X1 = 0,
		X2,
		X4,
		X8,
	};

	enum eMeasureCtrl
	{
		TEMPERATURE = 0x0E,
		PRESSURE = 0x14,

		START_CONV = 0x20,

		OVERSAMPLE_BIT = 0x06,
	};

	enum eChipID
	{
		CHIPID = 0x55
	};



	enum eRegisters
	{
		SOFT_RESET = 0xF0,
		CTRL_MEAS = 0xF4,

		OUT_MSB = 0xF6,
		OUT_LSB,
		OUT_XLSB,

		ID = 0xD0,

		CALIB0 = 0xAA,
		CALIB1,
		CALIB2,
		CALIB3,
		CALIB4,
		CALIB5,
		CALIB6,
		CALIB7,
		CALIB8,
		CALIB9,
		CALIB10,
		CALIB11,
		CALIB12,
		CALIB13,
		CALIB14,
		CALIB15,
		CALIB16,
		CALIB17,
		CALIB18,
		CALIB19,
		CALIB20,
		CALIB21,
	};




	BMP180(Devices::I2C& I2C, void(*delay_us)(uint16_t)) noexcept;
	void Init(uint8_t oversample);


	//in Pa
	uint32_t GetPressure();

	//With implied 1 dp.
	uint16_t GetTemperature();


private:


	void ReadCalibrationData();


	Devices::I2C& I2C;
	void (*delay_us)(uint16_t us);

	struct {
		int16_t AC1;
		int16_t AC2;
		int16_t AC3;
		uint16_t AC4;
		uint16_t AC5;
		uint16_t AC6;

		int16_t B1;
		int16_t B2;
		int16_t MB;
		int16_t MC;
		int16_t MD;
	} CalibrationData;

	int32_t UT;
	int32_t UP;

	uint16_t trueTemp;
	uint32_t truePressure;

	uint8_t oversampleMode;


};

}


#endif
