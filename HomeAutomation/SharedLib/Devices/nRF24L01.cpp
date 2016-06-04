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


#include <stdint.h>

#include <string.h>
#include "nRF24L01.h"

#include "Log.h"


namespace Devices
{


nRF24L01::nRF24L01(uint8_t rfChannel, SPI& SPIPort, GPIO& CE, GPIO& CSN, GPIO& IRQ) noexcept
		: rfChannel(rfChannel), SPIPort(SPIPort), CE(CE), CSN(CSN), IRQ(IRQ) { }



//Sets up port as output high, ready for request.
void nRF24L01::Init()
{
	CE.Init(Devices::GPIO::OUTPUT);
	CSN.Init(Devices::GPIO::OUTPUT);
	IRQ.Init(Devices::GPIO::INPUT);

	CE.SetOutput(Devices::GPIO::LOW);
	CSN.SetOutput(Devices::GPIO::HIGH);

	ChipDisable();

	state = STANDBY1;

	tx_payload = NULL;
	tx_payloadSize = 0;
	tx_payloadIndex = 0;

	//Enable IRQ for Max Retry, Data Ready, Data Sent.
	uint8_t data;
	data = (1<<EN_CRC) | (1<<PWR_UP);
	nRF24L01::WriteVerifyRegister(CONFIG, data);

	//Enable Auto Ack on all pipes
	data = (1<<ENAA_P0) | (1<<ENAA_P1) | (1<<ENAA_P2) | (1<<ENAA_P3) | (1<<ENAA_P4) | (1<<ENAA_P5);
	nRF24L01::WriteVerifyRegister(EN_AA, data);

	//Activate only P0 and P1 pipes.
	data = (1<<ERX_P0) | (1<<ERX_P1);
	nRF24L01::WriteVerifyRegister(EN_RXADDR, data);

	//Use 5 byte addresses
	data = 0x03;
	nRF24L01::WriteVerifyRegister(SETUP_AW, data);
	nRF24L01::WriteVerifyRegister(RF_CH, rfChannel);

	nRF24L01::WriteData(FLUSH_TX, NULL, 0);
	nRF24L01::WriteData(FLUSH_RX, NULL, 0);

	//Enable the W_TX_PAYLOAD_NOACK and W_RX_PAYLOAD command and Dynamic Payloads
	data = (1<< EN_DYN_ACK) | (1<<EN_DPL) | (1<<EN_ACK_PAY);
	nRF24L01::WriteVerifyRegister(FEATURE, data);

	//Enable dynamic payloads on all pipes.
	data = (1<<DPL_P0) | (1<<DPL_P1) | (1<<DPL_P2) | (1<<DPL_P3) | (1<<DPL_P4) | (1<<DPL_P5);
	nRF24L01::WriteVerifyRegister(DYNPD, data);

	//Set 15 retries at 750us delays
	data = (0x04 << ARD) | (0x0F << ARC);
	nRF24L01::WriteVerifyRegister(SETUP_RETR, data);

	//Set for 250kbps air speed, 0dBm power output
	data = (0x01 << RF_DR_LOW) | (0x00 << RF_DR_HIGH) | (0x03 << RF_PWR);
	nRF24L01::WriteVerifyRegister(RF_SETUP, data);

	//nRF24L01::WriteRegister(RX_ADDR_P1, local_address, 5);
}



//Set Pipe 1 to address supplied
uint8_t nRF24L01::Listen(uint8_t* address)
{
	if( state == STANDBY1 )
	{
		//If new address is required.
		if( memcmp(address, local_address, 5) )
		{
			memcpy(local_address , address, 5);
			nRF24L01::WriteRegister(RX_ADDR_P1, address, 5);
		}

		//Enable PRX
		uint8_t data = (1<<EN_CRC) | (1<<PWR_UP) | (1<<PRIM_RX);
		nRF24L01::WriteRegister(CONFIG, &data, 1);

		state = RECEIVING;
		ChipEnable();

		uint8_t debugAddr[5];
		nRF24L01::ReadRegister(RX_ADDR_P1, &debugAddr[0], 5);
		LOG_PRINT_HEXDUMP( API::Log::DEBUG, "Listening on: ", debugAddr, 5);

		return SUCCESS;
	}


	return NOT_READY;
}


uint8_t nRF24L01::Transmit(uint8_t* address, uint8_t* payload, uint16_t n)
{
	//Unit can go into transmit mode from receiving / Standby
	if( (state == STANDBY1) || (state == RECEIVING) )
	{
		ChipDisable();

		//If new address is required.
		if( memcmp(address, dest_address, 5) )
		{
			memcpy(dest_address , address, 5);
			nRF24L01::WriteRegister(TX_ADDR, address, 5);
			nRF24L01::WriteRegister(RX_ADDR_P0, address, 5);
		}

		tx_payload = payload;
		tx_payloadIndex = 0;
		tx_payloadSize = n;
		state = TRANSMITTING;

		TransmitPayload();
		return SUCCESS;
	}

	return NOT_READY;
}


//Blocking type send.
void nRF24L01::TransferSync()
{
	while( state == TRANSMITTING )
	{
		MainService();
	}
}

uint8_t nRF24L01::MainService()
{

	uint8_t statusByte;


	switch (state) {

		case STANDBY1:
			break;

		case TRANSMITTING:
			if ( IRQ.ReadInput() == IRQ_ACTIVE )
			{
				statusByte = nRF24L01::ReadRegister(NOP, NULL , 0);
				// On transmission completion.
				if( statusByte & (1<<TX_DS))
				{
					//Write 1 to Clear Data Sent INT to show we have handled it.
					statusByte |= (1<<TX_DS);
					nRF24L01::WriteRegister(STATUS, &statusByte, 1);

					//Continue to transmit all data, once it has finished, set state to IDLE
					if( tx_payloadIndex != tx_payloadSize)
					{
						TransmitPayload();
					}
					else
					{
						LOG_PRINT(API::Log::DEBUG, "Tx Complete");
						//Exit transmit mode Standby II back to Standby I
						ChipDisable();
						state = STANDBY1;
					}


				}


				if( statusByte & (1<<MAX_RT))
				{

					ChipDisable();

					LOG_PRINT(API::Log::ERR, "Max Retries exceeded");
					//Write 1 to Clear Max Retries INT to show we have handled it.
					statusByte |= (1<<MAX_RT);
					nRF24L01::WriteRegister(STATUS, &statusByte, 1);

					state = STANDBY1;



					uint8_t configState;
					nRF24L01::ReadRegister(CONFIG, &configState , 1);
					LOG_PRINT_HEX( API::Log::DEBUG, "Config: ", configState);


					nRF24L01::WriteData(FLUSH_TX, NULL, 0);

				}

			}
			break;

		case RECEIVING:
			if ( IRQ.ReadInput() == IRQ_ACTIVE )
			{
				statusByte = nRF24L01::ReadRegister(NOP, NULL , 0);
				if( statusByte & (1<<RX_DR))
				{
					LOG_PRINT( API::Log::DEBUG, "Packet Received");
					//Write 1 to Clear Max Retries INT to show we have handled it.
					statusByte |= (1<<RX_DR);
					nRF24L01::WriteRegister(STATUS, &statusByte, 1);

					state = DATA_RECEIVED;
				}
			}
			break;

		case DATA_RECEIVED:
			if ( IRQ.ReadInput() == IRQ_ACTIVE )
			{
				//We wait for the GetData function to be called. to clear this flag
			}
			else
			{
				state = RECEIVING;
			}

			break;


		default:
			ChipDisable();
			break;
	}


	return state;
}


bool nRF24L01::IsDataReady()
{
	uint8_t FIFOStatus;
	uint8_t statusByte = nRF24L01::ReadRegister(FIFO_STATUS, &FIFOStatus , 1);
	if( !(FIFOStatus & (1<<RX_EMPTY)))
	{
		return true;
	}
	else
	{
		return false;
	}
}


//Returns number of bytes received
uint8_t nRF24L01::GetData(uint8_t* buff)
{
	uint8_t bufIndex = 0;
	uint8_t payloadLength = 0;
	while( IsDataReady() )
	{
		nRF24L01::ReadData(R_RX_PL_WID, &payloadLength , 1);
		LOG_PRINT_DEC(API::Log::DEBUG, "Rx Count: ", payloadLength);

		if( payloadLength > FIFO_SIZE )
		{
			LOG_PRINT(API::Log::ERR, "DPL error");
			nRF24L01::WriteData(FLUSH_RX, NULL, 0);
		}

		nRF24L01::ReadData(R_RX_PAYLOAD, &buff[bufIndex] , payloadLength);
		bufIndex += payloadLength;
	}

	state = RECEIVING;

	return bufIndex;
}

void nRF24L01::PowerDown()
{
	//Enable IRQ for Max Retry, Data Ready, Data Sent.
	uint8_t data = (1<<EN_CRC);
	nRF24L01::WriteRegister(CONFIG, &data, 1);
	state = POWERDOWN;
	ChipDisable();
}




//====== Private Methods
void nRF24L01::TransmitPayload()
{
	uint8_t bytesRemaining = tx_payloadSize - tx_payloadIndex;
	uint8_t bytesToSend;
	uint8_t writeTxPayloadRegister = W_TX_PAYLOAD;

	if( state != TRANSMITTING )
	{
		LOG_PRINT(API::Log::ERR, "Not Ready");
		return;
	}

	if( ! en_ack )
	{
		writeTxPayloadRegister = W_TX_PAYLOAD_NACK;
	}

	// Fill up nRF24 internal TX FIFO buffers
	for( uint8_t i = 0; i < FIFO_COUNT; ++i )
	{
		if( tx_payloadIndex != tx_payloadSize )
		{
			uint8_t statusByte = nRF24L01::ReadRegister(NOP, NULL , 0);
			if( ! (statusByte & (1<<TX_FULL)) )
			{
				bytesToSend = (bytesRemaining >= FIFO_SIZE) ? FIFO_SIZE : bytesRemaining;
				nRF24L01::WriteRegister(writeTxPayloadRegister, &tx_payload[tx_payloadIndex], bytesToSend);
				tx_payloadIndex += bytesToSend;
				bytesRemaining = tx_payloadSize - tx_payloadIndex;
				LOG_PRINT_DEC(API::Log::DEBUG, "FIFO Write Size: ", bytesToSend);

			}
			else
			{
				LOG_PRINT( API::Log::ERR, "TX FIFO Full");
			}
		}
	}

	//Enable PTX
	uint8_t data = (1<<EN_CRC) | (1<<PWR_UP);
	nRF24L01::WriteRegister(CONFIG, &data, 1);

	//Initiate Transfer
	ChipEnable();
}






void nRF24L01::WriteVerifyRegister(uint8_t cmd, uint8_t data)
{
	uint8_t dataVerify;
	nRF24L01::WriteRegister(cmd, &data, 1);
	nRF24L01::ReadRegister(cmd, &dataVerify, 1);
	if( data != dataVerify )
	{
		LOG_PRINT_HEX( API::Log::ERR, "Verification Error = ", dataVerify);
		LOG_PRINT_HEX( API::Log::ERR, "Expected = ", data);
	}
}


uint8_t nRF24L01::WriteData(uint8_t cmd, uint8_t* data, uint8_t n)
{
	uint8_t statusByte = 0;

	Select();
	statusByte = SPIPort.TxByte(cmd);
	SPIPort.TxBlock(data, n);
	Deselect();

	return statusByte;
}

uint8_t nRF24L01::ReadData(uint8_t cmd, uint8_t* data, uint8_t n)
{
	uint8_t statusByte = 0;

	Select();
	statusByte = SPIPort.TxByte(cmd);
	LOG_PRINT_DEC( API::Log::DEBUG, "STA: ", statusByte);

	SPIPort.RxBlock(data, n);
	Deselect();

	return statusByte;
}



}






