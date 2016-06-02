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

/* Filename: 				i2c.c
 * Author:   				Adrian Gin (amg94@student.canterbury.ac.nz)
 * Created/Modified: 		21-06-06
 *
 * Description:
 *		These functions utilise the hardware TWIregisters
 * 	I2C / TWI serial routines for the AVR mpu's.
 * 	For AVR Core
 */

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdint.h>
#include <stdlib.h>

/* Include all the Status codes for the TWI module*/
#include <util/twi.h>
#include <util/delay.h>

#include "TWI.h"

#define I2C_DEBUG 0


/* i2cSetBitRate:
 * Sets up the i2c bitrate generator
 */
void i2cSetBitRate(uint8_t bitrate, uint8_t prescale)
{
	TWBR = bitrate;
	TWSR = (TWSR & (~TWI_PRESCALE_MASK)) | (prescale << TWPS0);
}



/* i2cInit:
 * Initialise the TWI modules by enabling the 
 * ACKnowledge bit and the TWI Enable bit
 *
 * Set the speed of the bitrate generator as well.
 */
void i2cInit(uint8_t bitrate, uint8_t prescale)
{
	i2cSetBitRate(bitrate, prescale);
	TWCR |= (1<<TWEA) | (1<<TWEN);
	
}

/* i2cDisable:
 * Disables the TWI Module access to the SDA and SCL pins
 *
 */
void i2cDisable(void)
{
	TWCR &=  ~(1<<TWEN);
}

#if I2C_DEBUG == 1
/* i2cError:
 * This is called when there is unexpected action on the TWI interface.
 * 
 * This function prints out the expected status code
 * followed by the actual status code. This function uses the hardware UART.
 * to transmit the data
 */
void i2cError(uint8_t expected_status)
{
	uint8_t error[3];
//	uartInit(BAUD9600, FAST);
	
	
	uartTxString_P(PSTR("\r\r\rAttemping to transmit the code: 0x"));
	utoa(TWDR, (char*)error, 16);
	uartTxString(error);
	
	
	/*Print expected status code*/
	uartTxString_P(PSTR("\r"
					"Expected Code was: 0x"));
	utoa(expected_status, (char*)error, 16);
	uartTxString(error);
	
	/* Print actual returned status code*/
	utoa((TWSR & TW_STATUS_MASK), (char*)error, 16);
	uartTxString_P(PSTR("\r"
					        "Actual Returned Code was: 0x"));
	uartTxString(error);				
}
#endif

/* i2cStart:
 * Place a start bit on the TWI Bus 
 *
 */	
/* TWINT is cleared when a 1 is written to it*/
/* TWI Operations take place when TWINT is cleared (set to 1) */
void i2cStart(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	/* Wait until the TW Interrupt flag is set (TWI Ready) */
	while( (TWCR & (1<<TWINT)) == 0);

}


/* i2cStop:
 * Place a stop bit on the TWI Bus 
 *
 */	
/* TWINT is cleared when a 1 is written to it*/
/* TWI Operations take place when TWINT is cleared (set to 1) */
void i2cStop(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
	/* Wait until the TW Interrupt flag is set (TWI Ready) */
	//while( !(TWCR & (1<<TWINT)) );
	
}



/* i2cTransmit:
 * The value in outbyte	is sent to the device 
 * The device should have already been addressed using
 * i2cAddress(...);
 */
void i2cTransmit(uint8_t outbyte)
{
	/* If the ACK was successfully received, then transmit the databyte */
	TWDR = outbyte;
	/* Ensure the start bit is zero, and enable the transmission & acknowledge */
	TWCR = (TWCR & (~I2C_TRANSMIT_MASK)) | (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	
	/* Wait until the TW Interrupt flag is set (TWI Ready) */
	while( (TWCR & (1<<TWINT)) == 0);
	
#if I2C_DEBUG == 1	
	/* Check that the Data was successfully sent */
	if( (TWSR & TW_STATUS_MASK) != TW_MT_DATA_ACK )
	{  
		i2cError(TW_MT_DATA_ACK);
	}
#endif

}


/* i2cTransmitBlock:
 * Outputs n number of bytes onto the TWI Bus.
 */
void i2cTransmitBlock(uint8_t* data_ptr, uint8_t n)
{
	uint8_t i;
	
	for( i = 0; i < n; i++)
	{
		i2cTransmit(data_ptr[i]);
	}

}



/* i2cAdrress:
 * Tranmits the passed address and RW bit to the TWI Bus lines
 */
void i2cAddress(uint8_t address, uint8_t rw_bit)
{
	/* Place a start condition on the TWI Bus and clear the interrupt*/
	i2cStart();

#if I2C_DEBUG == 1	
	/* Check that the start/repeated start bit was successfully sent */
	if(  (TWSR & TW_STATUS_MASK) != (TW_START) 
			&&
		  (TWSR & TW_STATUS_MASK) !=(TW_REP_START)  )
	{
		i2cError(TW_START);
	}
#endif	
	
	/* If start bit was successful, then send the address bit+R/W */
	TWDR = (address) + rw_bit;
	
	/* Ensure the start bit is zero, and enable the transmission */
	TWCR = (TWCR & (~I2C_TRANSMIT_MASK)) | (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	
	/* Wait until the TW Interrupt flag is set (TWI Ready) */
	while( (TWCR & (1<<TWINT)) == 0);

#if I2C_DEBUG == 1
	
	if( rw_bit == TW_WRITE)
	{
		/* Check that the SLA+W+ACK was successfully received */
		if( (TWSR & TW_STATUS_MASK) != TW_MT_SLA_ACK )
		{
			i2cError(TW_MT_SLA_ACK);
		}
	}
	else
	{
		/* Check that the SLA+R+ACK was successfully received */
		if( (TWSR & TW_STATUS_MASK) != TW_MR_SLA_ACK )
		{
			i2cError(TW_MR_SLA_ACK);
		}
	}
#endif
	
}





/* i2cRead:
 * The value in receiver buffer is returned.
 * This function should only be called after SLA+R+ACK has been sent */
/* Pass a 0 (LAST_BYTE) to the acknowledge bit to end the read
 * operation. 
 * Most I2C devices require the last byte to be read to end
 * with a NACK.
 */
 
uint8_t i2cRead(uint8_t acknowledge_bit)
{
	
	/* Setup the acknowledge bit and initiate the read */
	TWCR = (TWCR & (~I2C_RECEIVE_MASK)) | (acknowledge_bit<<TWEA) | (1<<TWINT) | (1<<TWEN);

	/* Wait until the TW Interrupt flag is set (TWI Ready) */
	while( (TWCR & (1<<TWINT)) == 0);

#if I2C_DEBUG == 1
	if( acknowledge_bit == ACK_BIT)
	{
		/* Check that the SLA+R+ACK was successfully received */
		if( (TWSR & TW_STATUS_MASK) != TW_MR_DATA_ACK )
		{
			i2cError(TW_MR_DATA_ACK);
		}
	}
	else
	{
		/* Check that the SLA+R+NACK was successfully received */
		if( (TWSR & TW_STATUS_MASK) != TW_MR_DATA_NACK )
		{
			i2cError(TW_MR_DATA_NACK);
		}
	}
#endif
	
	
	return TWDR;
	
}



/* i2cReadBlock:
 * Reads 'size' number of bytes to the passed pointer
 *
 */
void i2cReadBlock(uint8_t* destination_ptr, uint8_t size)
{
	uint8_t i;
	for( i = 0; i < (size-1); i++)
	{
		destination_ptr[i] = i2cRead(ACK_BIT);
	}
	
	destination_ptr[i] = i2cRead(NACK_BIT);
}











