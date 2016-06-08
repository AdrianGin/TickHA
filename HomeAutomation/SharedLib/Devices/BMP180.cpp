
#include <stdint.h>
#include "BMP180.h"
#include "Log.h"

namespace Devices
{

BMP180::BMP180(Devices::I2C& I2C, void(*delay_us)(uint16_t)) noexcept : I2C(I2C), delay_us(delay_us)
{

}

//Sets up port as output high, ready for request.
void BMP180::Init(uint8_t oversample)
{
	uint8_t id;

	oversampleMode = oversample;

	I2C.Address(I2CAddress, Devices::I2C::WRITE);
	I2C.Transmit(ID);
	I2C.Stop();

	I2C.Address(I2CAddress, Devices::I2C::READ);
	id = I2C.Read(Devices::I2C::NACK_BIT);
	I2C.Stop();

	if( id == CHIPID )
	{
		LOG_PRINT(API::Log::DBG, "BMP180 Init'ed");
	}
	else
	{
		LOG_PRINT(API::Log::DBG, "BMP180 Init Fail");
	}

	ReadCalibrationData();
	GetPressure();
}


void BMP180::ReadCalibrationData()
{
	uint8_t* calibrationData;

	I2C.Address(I2CAddress, Devices::I2C::WRITE);
	I2C.Transmit(CALIB0);
	I2C.Stop();

	I2C.Address(I2CAddress, Devices::I2C::READ);
	I2C.ReadBlock( (uint8_t*)&CalibrationData, sizeof(CalibrationData));

	calibrationData = (uint8_t*)&CalibrationData;
	for( uint8_t i = 0; i < sizeof(CalibrationData); i += 2 )
	{
		uint8_t tmp = calibrationData[i];
		calibrationData[i] = calibrationData[i + 1];
		calibrationData[i + 1] = tmp;
	}

	LOG_PRINT_HEXDUMP(API::Log::DBG, "CalibrationData = ", (uint8_t*)&CalibrationData, sizeof(CalibrationData));
}


uint32_t BMP180::GetPressure()
{
	// Obtain raw temperature
	I2C.Address(I2CAddress, Devices::I2C::WRITE);
	I2C.Transmit(CTRL_MEAS);
	I2C.Transmit(START_CONV | TEMPERATURE);
	I2C.Stop();

	delay_us(4500);

	I2C.Address(I2CAddress, Devices::I2C::WRITE);
	I2C.Transmit(OUT_MSB);
	I2C.Stop();

	I2C.Address(I2CAddress, Devices::I2C::READ);
	UT = I2C.Read(Devices::I2C::ACK_BIT) << 8;
	UT |= I2C.Read(Devices::I2C::NACK_BIT);
	I2C.Stop();

	// Obtain raw pressure
	I2C.Address(I2CAddress, Devices::I2C::WRITE);
	I2C.Transmit(CTRL_MEAS);
	I2C.Transmit(START_CONV | PRESSURE | (oversampleMode << OVERSAMPLE_BIT));
	I2C.Stop();

	delay_us(4500);

	I2C.Address(I2CAddress, Devices::I2C::WRITE);
	I2C.Transmit(OUT_MSB);
	I2C.Stop();

	I2C.Address(I2CAddress, Devices::I2C::READ);
	//MSB
	UP = (uint32_t)I2C.Read(Devices::I2C::ACK_BIT) << 16;
	//LSB
	UP |= (uint32_t)I2C.Read(Devices::I2C::ACK_BIT) << 8;
	//xLSB
	UP |= (uint32_t)I2C.Read(Devices::I2C::NACK_BIT);
	UP = UP >> (8-oversampleMode);
	I2C.Stop();

	LOG_PRINT_DEC(API::Log::DBG, "UT: ", UT);
	LOG_PRINT_DEC(API::Log::DBG, "UP: ", UP);

#define BMP180TEST 0
#if BMP180TEST
	UT = 27898;
	UP = 23843;

	CalibrationData.AC1 = 408;
	CalibrationData.AC2 = -72;

	CalibrationData.AC3 = -14383;
	CalibrationData.AC4 = 32741;
	CalibrationData.AC5 = 32757;
	CalibrationData.AC6 = 23153;

	CalibrationData.B1 = 6190;
	CalibrationData.B2 = 4;

	CalibrationData.MB = -32768;
	CalibrationData.MC = -8711;
	CalibrationData.MD = 2868;


#endif


	int32_t X1 = ((UT - CalibrationData.AC6) * CalibrationData.AC5) >> 15;
	LOG_PRINT_DEC(API::Log::DBG, "X1: ", X1);


	int32_t X2 = (((int32_t)CalibrationData.MC << 11) / (X1 + (int32_t)CalibrationData.MD));
	LOG_PRINT_DEC(API::Log::DBG, "X2: ", X2);

	int32_t B5 = X1 + X2;
	LOG_PRINT_DEC(API::Log::DBG, "B5: ", B5);

	trueTemp = (B5 + 8) >> 4;

	int32_t B6 = B5 - 4000;
	LOG_PRINT_DEC(API::Log::DBG, "B6: ", B6);

	X1 = (CalibrationData.B2 * ((B6 * B6) >> 12)) >> 11;
	LOG_PRINT_DEC(API::Log::DBG, "X1: ", X1);

	X2 = (CalibrationData.AC2 * B6) >> 11;
	LOG_PRINT_DEC(API::Log::DBG, "X2: ", X2);

	int32_t X3 = X1 + X2;
	LOG_PRINT_DEC(API::Log::DBG, "X3: ", X3);

	int32_t B3 = (((((int32_t)(CalibrationData.AC1) * 4) + X3) << oversampleMode) + 2) / 4;
	LOG_PRINT_DEC(API::Log::DBG, "B3: ", B3);

	LOG_PRINT_DEC(API::Log::DBG, "AC3: ", CalibrationData.AC3);
	X1 = (CalibrationData.AC3 * B6) >> 13;
	LOG_PRINT_DEC(API::Log::DBG, "X1: ", X1);

	LOG_PRINT_DEC(API::Log::DBG, "B1: ", CalibrationData.B1);
	X2 = (CalibrationData.B1 * ((B6 * B6) >> 12)) >> 16;
	LOG_PRINT_DEC(API::Log::DBG, "X2: ", X2);

	X3 = ((X1 + X2) + 2) >> 2;
	LOG_PRINT_DEC(API::Log::DBG, "X3: ", X3);

	LOG_PRINT_DEC(API::Log::DBG, "AC4: ", CalibrationData.AC4);
	uint32_t B4 = (CalibrationData.AC4 * (X3 + 32768)) >> 15;
	LOG_PRINT_DEC(API::Log::DBG, "B4: ", B4);


	LOG_PRINT_DEC(API::Log::DBG, "UP: ", UP);
	uint32_t B7 = (UP - B3) * (50000 >> oversampleMode);
	LOG_PRINT_DEC(API::Log::DBG, "B7: ", B7);

	int32_t p;
	if( B7 < 0x80000000)
	{
		p = (B7 * 2) / B4;
	}
	else
	{
		p = (B7 / B4) * 2;
	}


	X1 = (p >> 8) * (p >> 8);
	LOG_PRINT_DEC(API::Log::DBG, "X1: ", X1);
	X1 = (X1 * 3038) >> 16;
	LOG_PRINT_DEC(API::Log::DBG, "X1: ", X1);
	X2 = (-7357 * p) >> 16;
	LOG_PRINT_DEC(API::Log::DBG, "X2: ", X2);


	p = p + ((X1 + X2 + 3791) >> 4);
	truePressure = p;

	LOG_PRINT_DEC(API::Log::INFO, "Temperature is: ", trueTemp);
	LOG_PRINT_DEC(API::Log::INFO, "Pressure is: ", truePressure);


	return truePressure;
}


uint16_t BMP180::GetTemperature()
{
	return trueTemp;
}


}
