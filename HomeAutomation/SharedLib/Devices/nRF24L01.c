

#include <stdint.h>

#include <string.h>
#include "nRF24L01.h"

#include "debug.h"

#include <util/delay.h>

#define TXFIFO_SIZE (32)
#define TXFIFO_COUNT (3)

#define IRQ_ACTIVE (0)


//Set Pipe 1 to address supplied
uint8_t nRF24L01_Listen(nRF24L01_t* dev, uint8_t* address)
{
	if( dev->state == NRF24_IDLE )
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
	}

	return SUCCESS;
}


uint8_t nRF24L01_Transmit(nRF24L01_t* dev, uint8_t* address, uint8_t* payload, uint16_t n)
{
	if( dev->state == NRF24_IDLE )
	{
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

		Debug_String16bit("nRF24L01_Transmit", 0);
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
		return;
	}

	if( ! dev->en_ack )
	{
		writeTxPayloadRegister = W_TX_PAYLOAD_NACK;
	}

	// Fill up nRF24 internal TX FIFO buffers
	for( uint8_t i = 0; i < TXFIFO_COUNT; ++i )
	{

		if( dev->tx_payloadIndex != dev->tx_payloadSize )
		{

			uint8_t statusByte = nRF24L01_ReadRegister(dev, NOP, NULL , 0);
			if( ! (statusByte & (1<<TX_FULL)) )
			{
				bytesToSend = (bytesRemaining >= TXFIFO_SIZE) ? TXFIFO_SIZE : bytesRemaining;
				nRF24L01_WriteRegister(dev, writeTxPayloadRegister, &dev->tx_payload[dev->tx_payloadIndex], bytesToSend);

				dev->tx_payloadIndex += bytesToSend;
				bytesRemaining = dev->tx_payloadSize - dev->tx_payloadIndex;

			}
		}
	}

	//Enable PTX
	uint8_t data = (1<<EN_CRC) | (1<<PWR_UP);
	nRF24L01_WriteRegister(dev, CONFIG, &data, 1);

	//Initiate Transfer
	NRF24_TXMODE(dev);
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
	statusByte = nRF24L01_ReadRegister(dev, NOP, NULL , 0);

	switch (dev->state) {

		case NRF24_IDLE:
			break;

		case NRF24_TRANSMITTING:
			if ( dev->GetIRQState() == IRQ_ACTIVE )
			{
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
						//Exit transmit mode Standby II back to Standby I
						NRF24_RXMODE(dev);
						dev->state = NRF24_IDLE;

					}

					return SUCCESS;
				}


				if( statusByte & (1<<MAX_RT))
				{
					//Write 1 to Clear Max Retries INT to show we have handled it.
					statusByte |= (1<<MAX_RT);
					nRF24L01_WriteRegister(dev, STATUS, &statusByte, 1);

					dev->state = NRF24_IDLE;

					return RETRY_FAIL;
				}

			}
			break;

		case NRF24_RECEIVING:
			break;


		default:
			NRF24_RXMODE(dev);
			break;
	}


	return 0;

}



void nRF24L01_HandleIRQ(nRF24L01_t* dev)
{
	uint8_t statusByte;
	statusByte = nRF24L01_ReadRegister(dev, NOP, NULL , 0);
}



//Sets up port as output high, ready for request.
void nRF24L01_Init(nRF24L01_t* dev)
{
	dev->IOInit();

	dev->state = NRF24_IDLE;

	dev->tx_payload = NULL;
	dev->tx_payloadSize = 0;
	dev->tx_payloadIndex = 0;

	//Enable IRQ for Max Retry, Data Ready, Data Sent.
	uint8_t data = (1<<EN_CRC) | (1<<PWR_UP);
	nRF24L01_WriteRegister(dev, CONFIG, &data, 1);

	//Enable Auto Ack on all pipes
	data = (1<<ENAA_P0) | (1<<ENAA_P1) | (1<<ENAA_P2) | (1<<ENAA_P3) | (1<<ENAA_P4) | (1<<ENAA_P5);
	nRF24L01_WriteData(dev, EN_AA, &data, 1);

	//Activate only P0 and P1 pipes.
	data = (1<<ENAA_P0) | (1<<ENAA_P1);
	nRF24L01_WriteData(dev, EN_RXADDR, &data, 1);

	//Use 5 byte addresses
	data = 0x03;
	nRF24L01_WriteData(dev, SETUP_AW, &data, 1);

	nRF24L01_WriteData(dev, RF_CH, &dev->rfChannel, 1);

	nRF24L01_WriteData(dev, FLUSH_TX, NULL, 0);
	nRF24L01_WriteData(dev, FLUSH_RX, NULL, 0);

	uint8_t data2;
	nRF24L01_ReadRegister(dev , CONFIG,  &data2, 1);

	//Enable the W_TX_PAYLOAD_NOACK command
	data = (1<< EN_DYN_ACK);
	nRF24L01_WriteRegister(dev, FEATURE, &data, 1);

	//Set 15 retries at 750us delays
	data = (0x02 << ARD) | (0x0F << ARC);
	nRF24L01_WriteRegister(dev, SETUP_RETR, &data, 1);

	//Set for 250kbps air speed, 0dBm power output
	data = (0x01 << RF_DR_LOW) | (0x02 << RF_DR_HIGH) | (0x03 << RF_PWR);
	nRF24L01_WriteRegister(dev, RF_SETUP, &data, 1);

}


void nRF24L01_PowerDown(nRF24L01_t* dev)
{
	//Enable IRQ for Max Retry, Data Ready, Data Sent.
	uint8_t data = (1<<EN_CRC);
	nRF24L01_WriteRegister(dev, CONFIG, &data, 1);

	dev->state = NRF24_POWERDOWN;
}



uint8_t nRF24L01_WriteData(nRF24L01_t* dev, uint8_t cmd, uint8_t* data, uint8_t n)
{
	uint8_t statusByte;

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
	uint8_t statusByte;

	NRF24_SELECT(dev);
	statusByte = dev->SPIWrite(cmd);
	Debug_String16bit("STA: ", statusByte);

	for( uint8_t i = 0; i < n; ++i)
	{
		data[i] = dev->SPIWrite(0xFF);
	}

	NRF24_DESELECT(dev);

	return statusByte;
}








