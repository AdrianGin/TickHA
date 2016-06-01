

#include <stdint.h>

#include <string.h>
#include "nRF24L01.h"

#include "debug.h"

#define FIFO_SIZE (32)
#define FIFO_COUNT (3)

#define IRQ_ACTIVE (0)


//Set Pipe 1 to address supplied
uint8_t nRF24L01_Listen(nRF24L01_t* dev, uint8_t* address)
{
	if( dev->state == NRF24_STANDBY1 )
	{
		//If new address is required.
		if( memcmp(address, dev->local_address, 5) )
		{
			memcpy(dev->local_address , address, 5);
			nRF24L01_WriteRegister(dev, RX_ADDR_P1, address, 5);
		}

		//Enable PRX
		uint8_t data = (1<<EN_CRC) | (1<<PWR_UP) | (1<<PRIM_RX);
		nRF24L01_WriteRegister(dev, CONFIG, &data, 1);

		dev->state = NRF24_RECEIVING;
		dev->ChipEnable(CE_ENABLE);

		uint8_t debugAddr[5];
		nRF24L01_ReadRegister(dev, RX_ADDR_P1, &debugAddr[0], 5);
		LOG_PRINT_HEXDUMP(LOG_DEBUG, "Listening on: ", debugAddr, 5);

		return SUCCESS;
	}


	return NOT_READY;
}


uint8_t nRF24L01_Transmit(nRF24L01_t* dev, uint8_t* address, uint8_t* payload, uint16_t n)
{
	//Unit can go into transmit mode from receiving / Standby
	if( (dev->state == NRF24_STANDBY1) || (dev->state == NRF24_RECEIVING) )
	{
		dev->ChipEnable(CE_DISABLE);

		//If new address is required.
		if( memcmp(address, dev->dest_address, 5) )
		{
			memcpy(dev->dest_address , address, 5);
			nRF24L01_WriteRegister(dev, TX_ADDR, address, 5);
			nRF24L01_WriteRegister(dev, RX_ADDR_P0, address, 5);
		}

		dev->tx_payload = payload;
		dev->tx_payloadIndex = 0;
		dev->tx_payloadSize = n;
		dev->state = NRF24_TRANSMITTING;

		nRF24L01_TransmitPayload(dev);
		return SUCCESS;
	}

	return NOT_READY;
}


void nRF24L01_TransmitPayload(nRF24L01_t* dev)
{
	uint8_t bytesRemaining = dev->tx_payloadSize - dev->tx_payloadIndex;
	uint8_t bytesToSend;
	uint8_t writeTxPayloadRegister = W_TX_PAYLOAD;

	if( dev->state != NRF24_TRANSMITTING )
	{
		LOG_PRINT(LOG_ERR, "Not Ready");
		return;
	}

	if( ! dev->en_ack )
	{
		writeTxPayloadRegister = W_TX_PAYLOAD_NACK;
	}

	// Fill up nRF24 internal TX FIFO buffers
	for( uint8_t i = 0; i < FIFO_COUNT; ++i )
	{
		if( dev->tx_payloadIndex != dev->tx_payloadSize )
		{
			uint8_t statusByte = nRF24L01_ReadRegister(dev, NOP, NULL , 0);
			if( ! (statusByte & (1<<TX_FULL)) )
			{

				bytesToSend = (bytesRemaining >= FIFO_SIZE) ? FIFO_SIZE : bytesRemaining;
				nRF24L01_WriteRegister(dev, writeTxPayloadRegister, &dev->tx_payload[dev->tx_payloadIndex], bytesToSend);
				dev->tx_payloadIndex += bytesToSend;
				bytesRemaining = dev->tx_payloadSize - dev->tx_payloadIndex;

				LOG_PRINT_DEC(LOG_DEBUG, "FIFO Write Size: ", bytesToSend);

			}
			else
			{
				LOG_PRINT(LOG_ERR, "TX FIFO Full");
			}
		}
	}

	uint8_t debugAddr[5];
	nRF24L01_ReadRegister(dev, TX_ADDR, &debugAddr[0], 5);
	LOG_PRINT_HEXDUMP(LOG_DEBUG, "TXADDR: ", &debugAddr[0], 5);
	nRF24L01_ReadRegister(dev, RX_ADDR_P0, &debugAddr[0], 5);
	LOG_PRINT_HEXDUMP(LOG_DEBUG, "RX P0: ", &debugAddr[0], 5);

	//Enable PTX
	uint8_t data = (1<<EN_CRC) | (1<<PWR_UP);
	nRF24L01_WriteRegister(dev, CONFIG, &data, 1);

	//Initiate Transfer
	dev->ChipEnable(CE_ENABLE);
}


//Blocking type send.
void nRF24L01_TransferSync(nRF24L01_t* dev)
{
	while( dev->state == NRF24_TRANSMITTING )
	{
		nRF24L01_MainService(dev);
	}
}

uint8_t nRF24L01_MainService(nRF24L01_t* dev)
{

	uint8_t statusByte;


	switch (dev->state) {

		case NRF24_STANDBY1:
			break;

		case NRF24_TRANSMITTING:
			if ( dev->GetIRQState() == IRQ_ACTIVE )
			{
				statusByte = nRF24L01_ReadRegister(dev, NOP, NULL , 0);
				// On transmission completion.
				if( statusByte & (1<<TX_DS))
				{
					//Write 1 to Clear Data Sent INT to show we have handled it.
					statusByte |= (1<<TX_DS);
					nRF24L01_WriteRegister(dev, STATUS, &statusByte, 1);

					//Continue to transmit all data, once it has finished, set state to IDLE
					if( dev->tx_payloadIndex != dev->tx_payloadSize)
					{
						nRF24L01_TransmitPayload(dev);
					}
					else
					{
						LOG_PRINT(LOG_DEBUG, "Tx Complete");
						//Exit transmit mode Standby II back to Standby I
						dev->ChipEnable(CE_DISABLE);
						dev->state = NRF24_STANDBY1;
					}


				}


				if( statusByte & (1<<MAX_RT))
				{

					dev->ChipEnable(CE_DISABLE);

					LOG_PRINT(LOG_ERR, "Max Retries exceeded");
					//Write 1 to Clear Max Retries INT to show we have handled it.
					statusByte |= (1<<MAX_RT);
					nRF24L01_WriteRegister(dev, STATUS, &statusByte, 1);

					dev->state = NRF24_STANDBY1;



					uint8_t configState;
					nRF24L01_ReadRegister(dev, CONFIG, &configState , 1);
					LOG_PRINT_HEX(LOG_DEBUG, "Config: ", configState);


					nRF24L01_WriteData(dev, FLUSH_TX, NULL, 0);

				}

			}
			break;

		case NRF24_RECEIVING:
			if ( dev->GetIRQState() == IRQ_ACTIVE )
			{
				statusByte = nRF24L01_ReadRegister(dev, NOP, NULL , 0);
				if( statusByte & (1<<RX_DR))
				{
					LOG_PRINT(LOG_DEBUG, "Packet Received");
					//Write 1 to Clear Max Retries INT to show we have handled it.
					statusByte |= (1<<RX_DR);
					nRF24L01_WriteRegister(dev, STATUS, &statusByte, 1);

					dev->state = NRF24_DATA_RECEIVED;
				}
			}
			break;

		case NRF24_DATA_RECEIVED:
			if ( dev->GetIRQState() == IRQ_ACTIVE )
			{
				//We wait for the GetData function to be called. to clear this flag
			}
			else
			{
				dev->state = NRF24_RECEIVING;
			}

			break;


		default:
			dev->ChipEnable(CE_DISABLE);
			break;
	}


	return dev->state;
}


uint8_t nRF24L01_IsDataReady(nRF24L01_t* dev)
{
	uint8_t FIFOStatus;
	uint8_t statusByte = nRF24L01_ReadRegister(dev, FIFO_STATUS, &FIFOStatus , 1);
	if( !(FIFOStatus & (1<<RX_EMPTY)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//Returns number of bytes received
uint8_t nRF24L01_GetData(nRF24L01_t* dev, uint8_t* buff)
{
	uint8_t bufIndex = 0;
	uint8_t payloadLength = 0;
	while( nRF24L01_IsDataReady(dev) )
	{
		nRF24L01_ReadData(dev, R_RX_PL_WID, &payloadLength , 1);
		LOG_PRINT_DEC(LOG_DEBUG, "Rx Count: ", payloadLength);

		if( payloadLength > FIFO_SIZE )
		{
			LOG_PRINT(LOG_ERR, "DPL error");
			nRF24L01_WriteData(dev, FLUSH_RX, NULL, 0);
		}

		nRF24L01_ReadData(dev, R_RX_PAYLOAD, &buff[bufIndex] , payloadLength);
		bufIndex += payloadLength;
	}

	dev->state = NRF24_RECEIVING;

	return bufIndex;
}


void nRF24L01_HandleIRQ(nRF24L01_t* dev)
{
	uint8_t statusByte;
	statusByte = nRF24L01_ReadRegister(dev, NOP, NULL , 0);
}

void nRF24L01_WriteVerifyRegister(nRF24L01_t* dev, uint8_t cmd, uint8_t data)
{
	uint8_t dataVerify;
	nRF24L01_WriteRegister(dev, cmd, &data, 1);
	nRF24L01_ReadRegister(dev, cmd, &dataVerify, 1);
	if( data == dataVerify )
	{
		LOG_PRINT_HEX(LOG_ERR, "Verification Error = ", dataVerify);
		LOG_PRINT_HEX(LOG_ERR, "Expected = ", data);
	}
}

//Sets up port as output high, ready for request.
void nRF24L01_Init(nRF24L01_t* dev)
{
	dev->IOInit();

	dev->ChipEnable(CE_DISABLE);

	dev->state = NRF24_STANDBY1;

	dev->tx_payload = NULL;
	dev->tx_payloadSize = 0;
	dev->tx_payloadIndex = 0;

	//Enable IRQ for Max Retry, Data Ready, Data Sent.
	uint8_t data;
	data = (1<<EN_CRC) | (1<<PWR_UP);
	nRF24L01_WriteVerifyRegister(dev, CONFIG, data);

	//Enable Auto Ack on all pipes
	data = (1<<ENAA_P0) | (1<<ENAA_P1) | (1<<ENAA_P2) | (1<<ENAA_P3) | (1<<ENAA_P4) | (1<<ENAA_P5);
	nRF24L01_WriteVerifyRegister(dev, EN_AA, data);

	//Activate only P0 and P1 pipes.
	data = (1<<ERX_P0) | (1<<ERX_P1);
	nRF24L01_WriteVerifyRegister(dev, EN_RXADDR, data);

	//Use 5 byte addresses
	data = 0x03;
	nRF24L01_WriteVerifyRegister(dev, SETUP_AW, data);
	nRF24L01_WriteVerifyRegister(dev, RF_CH, dev->rfChannel);

	nRF24L01_WriteData(dev, FLUSH_TX, NULL, 0);
	nRF24L01_WriteData(dev, FLUSH_RX, NULL, 0);

	//Enable the W_TX_PAYLOAD_NOACK and W_RX_PAYLOAD command and Dynamic Payloads
	data = (1<< EN_DYN_ACK) | (1<<EN_DPL) | (1<<EN_ACK_PAY);
	//nRF24L01_WriteVerifyRegister(dev, FEATURE, data);

	//Enable dynamic payloads on all pipes.
	data = (1<<DPL_P0) | (1<<DPL_P1) | (1<<DPL_P2) | (1<<DPL_P3) | (1<<DPL_P4) | (1<<DPL_P5);
	//nRF24L01_WriteVerifyRegister(dev, DYNPD, data);

	//Set 15 retries at 750us delays
	data = (0x04 << ARD) | (0x0F << ARC);
	nRF24L01_WriteVerifyRegister(dev, SETUP_RETR, data);

	//Set for 250kbps air speed, 0dBm power output
	data = (0x00 << RF_DR_LOW) | (0x00 << RF_DR_HIGH) | (0x03 << RF_PWR);
	nRF24L01_WriteVerifyRegister(dev, RF_SETUP, data);


	nRF24L01_WriteRegister(dev, RX_ADDR_P1, dev->local_address, 5);


}


void nRF24L01_PowerDown(nRF24L01_t* dev)
{
	//Enable IRQ for Max Retry, Data Ready, Data Sent.
	uint8_t data = (1<<EN_CRC);
	nRF24L01_WriteRegister(dev, CONFIG, &data, 1);
	dev->state = NRF24_POWERDOWN;

	dev->ChipEnable(CE_DISABLE);
}



uint8_t nRF24L01_WriteData(nRF24L01_t* dev, uint8_t cmd, uint8_t* data, uint8_t n)
{
	uint8_t statusByte = 0;

	NRF24_SELECT(dev);
	statusByte = dev->SPIWrite(cmd);

	for( uint8_t i = 0; i < n; ++i)
	{
		dev->SPIWrite(data[i]);
	}
	NRF24_DESELECT(dev);

	return statusByte;
}

uint8_t nRF24L01_ReadData(nRF24L01_t* dev , uint8_t cmd, uint8_t* data, uint8_t n)
{
	uint8_t statusByte = 0;

	NRF24_SELECT(dev);
	statusByte = dev->SPIWrite(cmd);
	LOG_PRINT_DEC(LOG_DEBUG, "STA: ", statusByte);

	for( uint8_t i = 0; i < n; ++i)
	{
		data[i] = dev->SPIWrite(0xFF);
	}

	NRF24_DESELECT(dev);

	return statusByte;
}








