#ifndef _nRF24L01_DEVICE
#define _nRF24L01_DEVICE

#include <stdint.h>
#include <util/delay.h>

typedef struct nRF24L01
{

	uint8_t (*SPIRead)(void);
	uint8_t (*SPIWrite)(uint8_t byte);
	void 	(*ChipSelect)(uint8_t state);

	void 	(*ChipEnableRXTX)(uint8_t state);

	void 	(*IRQCallback)(uint8_t state);

} nRF24L01_t;


#endif
