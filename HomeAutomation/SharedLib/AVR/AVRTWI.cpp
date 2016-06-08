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
#include "AVRTWI.h"

#include <stdint.h>
#include <stdlib.h>

/* Include all the Status codes for the TWI module*/
#include <util/twi.h>
#include <util/delay.h>


namespace AVR
{

#define I2C_DEBUG 0


/* AVRTWI::SetBitRate:
 * Sets up the AVRTWI:: bitrate generator
 */
void AVRTWI::SetBitRate(uint8_t br)
{
	uint8_t bitrate = TWI_100KHZ;
	if( bitrate == Devices::I2C::CLK_400KHZ )
	{
		bitrate = TWI_400KHZ;
	}

	TWBR = bitrate;
	TWSR = (TWSR & (~TWI_PRESCALE_MASK)) | (0 << TWPS0);
}



/* AVRTWI::Init:
 * Initialise the TWI modules by enabling the 
 * ACKnowledge bit and the TWI Enable bit
 *
 * Set the speed of the bitrate generator as well.
 */
void AVRTWI::Init(uint8_t bitrate)
{
	AVRTWI::SetBitRate(bitrate);
	TWCR |= (1<<TWEA) | (1<<TWEN);
	
}

/* AVRTWI::Disable:
 * Disables the TWI Module access to the SDA and SCL pins
 *
 */
void AVRTWI::Disable(void)
{
	TWCR &=  ~(1<<TWEN);
}

#if I2C_DEBUG == 1
/* AVRTWI::Error:
 * This is called when there is unexpected action on the TWI interface.
 * 
 * This function prints out the expected status code
 * followed by the actual status code. This function uses the hardware UART.
 * to transmit the data
 */
void AVRTWI::Error(uint8_t expected_status)
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

/* AVRTWI::Start:
 * Place a start bit on the TWI Bus 
 *
 */	
/* TWINT is cleared when a 1 is written to it*/
/* TWI Operations take place when TWINT is cleared (set to 1) */
void AVRTWI::Start(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	/* Wait until the TW Interrupt flag is set (TWI Ready) */
	while( (TWCR & (1<<TWINT)) == 0);

}


/* AVRTWI::Stop:
 * Place a stop bit on the TWI Bus 
 *
 */	
/* TWINT is cleared when a 1 is written to it*/
/* TWI Operations take place when TWINT is cleared (set to 1) */
void AVRTWI::Stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
	/* Wait until the TW Interrupt flag is set (TWI Ready) */
	//while( !(TWCR & (1<<TWINT)) );
	
}



/* AVRTWI::Transmit:
 * The value in outbyte	is sent to the device 
 * The device should have already been addressed using
 * AVRTWI::Address(...);
 */
void AVRTWI::Transmit(uint8_t outbyte)
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
		AVRTWI::Error(TW_MT_DATA_ACK);
	}
#endif

}





/* AVRTWI::Adrress:
 * Tranmits the passed address and RW bit to the TWI Bus lines
 */
void AVRTWI::Address(uint8_t address, uint8_t rw_bit)
{
	/* Place a start condition on the TWI Bus and clear the interrupt*/
	AVRTWI::Start();

#if I2C_DEBUG == 1	
	/* Check that the start/repeated start bit was successfully sent */
	if(  (TWSR & TW_STATUS_MASK) != (TW_START) 
			&&
		  (TWSR & TW_STATUS_MASK) !=(TW_REP_START)  )
	{
		AVRTWI::Error(TW_START);
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
			AVRTWI::Error(TW_MT_SLA_ACK);
		}
	}
	else
	{
		/* Check that the SLA+R+ACK was successfully received */
		if( (TWSR & TW_STATUS_MASK) != TW_MR_SLA_ACK )
		{
			AVRTWI::Error(TW_MR_SLA_ACK);
		}
	}
#endif
	
}





/* AVRTWI::Read:
 * The value in receiver buffer is returned.
 * This function should only be called after SLA+R+ACK has been sent */
/* Pass a 0 (LAST_BYTE) to the acknowledge bit to end the read
 * operation. 
 * Most I2C devices require the last byte to be read to end
 * with a NACK.
 */
 
uint8_t AVRTWI::Read(uint8_t acknowledge_bit)
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
			AVRTWI::Error(TW_MR_DATA_ACK);
		}
	}
	else
	{
		/* Check that the SLA+R+NACK was successfully received */
		if( (TWSR & TW_STATUS_MASK) != TW_MR_DATA_NACK )
		{
			AVRTWI::Error(TW_MR_DATA_NACK);
		}
	}
#endif
	
	
	return TWDR;
	
}






}





