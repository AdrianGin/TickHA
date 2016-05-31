#ifndef _nRF24L01_DEVICE
#define _nRF24L01_DEVICE

#include <stdint.h>
#include <util/delay.h>
#include "nRF24L01_defs.h"


typedef enum
{
	SUCCESS = 0,
	NOT_READY,
	RETRY_NEEDED,
	RETRY_FAIL,
} eNRF24err_t;

typedef enum
{
	NRF24_IDLE = 0, //Standby I Mode
	NRF24_TRANSMITTING,
	NRF24_RECEIVING,

	NRF24_TRANSMIT_ERROR,
	NRF24_RECEIVE_ERROR,

	NRF24_POWERDOWN,
} eNRF24states_t;



#define CE_TX (1)
#define CE_RX (0)


typedef struct nRF24L01_t nRF24L01_t;
struct nRF24L01_t
{

	uint8_t rfChannel;
	uint8_t state;

	//Address of this device
	uint8_t local_address[5];

	//Address of the destination device
	uint8_t dest_address[5];
	uint8_t en_ack;

	uint8_t* tx_payload;
	uint16_t tx_payloadIndex; //Keeps track of how many bytes we have sent
	uint16_t tx_payloadSize;


	void    (*IOInit)(void);
	uint8_t (*SPIRead)(void);
	uint8_t (*SPIWrite)(uint8_t byte);
	void 	(*ChipSelect)(uint8_t state);
	void 	(*ChipEnable)(uint8_t state);
	uint8_t (*GetIRQState)(void);

	void 	(*IRQCallback)(nRF24L01_t* dev);

};


void nRF24L01_Init(nRF24L01_t* dev);
uint8_t nRF24L01_WriteData(nRF24L01_t* dev, uint8_t cmd, uint8_t* data, uint8_t n);
uint8_t nRF24L01_ReadData(nRF24L01_t* dev , uint8_t cmd, uint8_t* data, uint8_t n);

void nRF24L01_SetupTransmit(nRF24L01_t* dev, uint8_t* address, uint8_t* payload, uint16_t n);
uint8_t nRF24L01_Transmit(nRF24L01_t* dev, uint8_t* address, uint8_t* payload, uint16_t n);

void nRF24L01_TransmitPayload(nRF24L01_t* dev);
void nRF24L01_TransferSync(nRF24L01_t* dev);

uint8_t nRF24L01_MainService(nRF24L01_t* dev);

void nRF24L01_PowerDown(nRF24L01_t* dev);


static inline void nRF24L01_SetAckState(nRF24L01_t* dev, uint8_t state)
{
	dev->en_ack = state;
}

static inline uint8_t nRF24L01_WriteRegister(nRF24L01_t* dev, uint8_t reg, uint8_t* data, uint8_t n)
{
	return nRF24L01_WriteData(dev, reg | W_REGISTER, data, n);
}

static inline uint8_t nRF24L01_ReadRegister(nRF24L01_t* dev, uint8_t reg, uint8_t* data, uint8_t n)
{
	return nRF24L01_ReadData(dev, reg | R_REGISTER, data, n);
}


static inline void NRF24_SELECT(nRF24L01_t* dev)
{
	dev->ChipSelect(0);
}

static inline void NRF24_DESELECT(nRF24L01_t* dev)
{
	dev->ChipSelect(1);
}

static inline void NRF24_RXMODE(nRF24L01_t* dev)
{
	dev->ChipEnable(CE_RX);
}

static inline void NRF24_TXMODE(nRF24L01_t* dev)
{
	dev->ChipEnable(CE_TX);
}


static inline void NRF24_CE_PULSE(nRF24L01_t* dev)
{
	dev->ChipEnable(CE_TX);
	_delay_us(11);
	dev->ChipEnable(CE_RX);
}

#endif
