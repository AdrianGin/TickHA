

#include <stdint.h>
#include "nRF24L01.h"
#include "nRF24L01_defs.h"

#include "USARTn.h"






//Sets up port as output high, ready for request.
void nRF24L01_Init(nRF24L01_t* dev);



uint8_t nRF24L01_WriteData(nRF24L01_t* dev, uint8_t cmd, uint8_t* data, uint8_t n);
uint8_t nRF24L01_ReadData(nRF24L01_t* dev , uint8_t cmd, uint8_t* data, uint8_t n);

uint8_t nRF24L01_WriteSPIByte(nRF24L01_t* dev, uint8_t byte);
