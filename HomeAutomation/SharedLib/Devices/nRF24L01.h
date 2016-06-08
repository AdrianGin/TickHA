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


#ifndef _nRF24L01_DEVICE
#define _nRF24L01_DEVICE

#include <stdint.h>
#include <util/delay.h>
#include "nRF24L01_defs.h"
#include "GPIO.h"
#include "SPI.h"

namespace Devices
{


class nRF24L01
{
public:
	enum eErrors
	{
		SUCCESS = 0,
		NOT_READY,
		RETRY_NEEDED,
		RETRY_FAIL,
	};

	enum eStates
	{
		STANDBY1 = 0, //Standby I Mode
		TRANSMITTING,
		RECEIVING,
		DATA_RECEIVED,
		TRANSMIT_ERROR,
		RECEIVE_ERROR,
		POWERDOWN,
	};

	enum eCEStates
	{
		ENABLE = 0,
		DISABLE,
	};

	enum eIRQState
	{
		IRQ_ACTIVE = 0,
	};

	enum eFIFOInfo
	{
		FIFO_COUNT = 3,
		FIFO_SIZE = 32,
	};

	nRF24L01(uint8_t rfChannel, SPI& SPIPort, GPIO& CE, GPIO& CSN, GPIO& IRQ) noexcept;
	void Init(void);

	uint8_t Listen(uint8_t* address);
	uint8_t Transmit(uint8_t* address, uint8_t* payload, uint16_t n);
	//Finish transmitting packets
	void TransferSync();

	//Call this in the main loop of the main program's while loop
	uint8_t MainService();


	//True if there is RX packets in the FIFO
	bool IsDataReady();
	//Returns number of bytes received
	uint8_t GetData(uint8_t* buff);

	void PowerDown();

	inline void SetAckState(uint8_t state)
	{
		en_ack = state;
	}

	uint8_t GetState(void) {return state;}



private:
	uint8_t state;
	uint8_t rfChannel;
	//Address of this device
	uint8_t local_address[5];

	//Address of the destination device
	uint8_t dest_address[5];
	uint8_t en_ack;

	uint8_t* tx_payload;
	uint16_t tx_payloadIndex; //Keeps track of how many bytes we have sent
	uint16_t tx_payloadSize;

	SPI& SPIPort;
	GPIO& CE;
	GPIO& CSN;
	GPIO& IRQ;



	uint8_t SPIRead(void);
	uint8_t SPIWrite(uint8_t byte);

	uint8_t WriteData(uint8_t cmd, uint8_t* data, uint8_t n);
	uint8_t ReadData(uint8_t cmd, uint8_t* data, uint8_t n);

	void WriteVerifyRegister(uint8_t cmd, uint8_t data);

	void TransmitPayload();


	inline uint8_t WriteRegister(uint8_t reg, uint8_t* data, uint8_t n)
	{
		return WriteData(reg | W_REGISTER, data, n);
	}

	inline uint8_t ReadRegister(uint8_t reg, uint8_t* data, uint8_t n)
	{
		return ReadData(reg | R_REGISTER, data, n);
	}

	inline void ChipEnable()
	{
		CE.SetOutput(Devices::GPIO::HIGH);
	}

	inline void ChipDisable()
	{
		CE.SetOutput(Devices::GPIO::LOW);
	}

	inline void Select()
	{
		CSN.SetOutput(Devices::GPIO::LOW);
	}

	inline void Deselect()
	{
		CSN.SetOutput(Devices::GPIO::HIGH);
	}

};

}


#endif
