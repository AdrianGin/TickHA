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

/* Filename: 				i2c.h
 * Author:   				Adrian Gin (amg94@student.canterbury.ac.nz)
 * Created/Modified: 		21-06-07
 *
 * Description:
 *		These functions utilise the hardware TWIregisters
 * 	I2C / TWI serial routines for the AVR mpu's.
 * 	For AVR Core
 */

/** \file i2c.h
	 \brief TWI / I2C Drivers for the AVR Core.
*/

/**	 

	\ingroup avr_peripheral
 	\defgroup i2c I2C Hardware Driver.
 	\code #include "i2c.h" \endcode

 	 
	 \par Description
	 I2C Communications protocol for the AVR core.
 	 
*/
//@{

#ifndef	_AVR_TWI_H
#define	_AVR_TWI_H

#include <avr/io.h>

#include <stdint.h>
#include <util/twi.h>

#include "I2C.h"


#define	TWI_PRESCALE_MASK		(0x03)
#define	TWI_PRESCALE_0			(0x00)
#define	TWI_PRESCALE_1			(0x01)
#define	TWI_PRESCALE_2		(0x02)
#define	TWI_PRESCALE_3		(0x03)

#if F_CPU == 8000000UL
#define	TWI_100KHZ			(32)
#define	TWI_400KHZ			(2)
#endif

#if F_CPU == 16000000UL
#define	TWI_100KHZ			(72)
#define	TWI_400KHZ			(12)
#endif


#define	TWI_STATUS_MASK		0xF8

#define	I2C_TRANSMIT_MASK 	((1<<TWINT) | (1<<TWSTA) | (1<<TWEA))
#define	I2C_RECEIVE_MASK 		((1<<TWINT) | (1<<TWSTO) | (1<<TWEA))

namespace AVR
{

class AVRTWI : public Devices::I2C
{

public:

	AVRTWI(void) noexcept {};

	/** SetBitRate:
	 * Sets up the  bitrate generator
	 */
	void SetBitRate(uint8_t bitrate);


	/** Init:
	 * Initialise the I2C modules by enabling the
	 * ACKnowledge bit and the I2C Enable bit
	 *
	 * Set the speed of the bitrate generator as well.
	 */

	void Init(uint8_t bitrate);

	/** Disable:
	 * Disables the I2C Module access to the SDA and SCL pins
	 *
	 */
	void Disable(void);

	/** Error:
	 * This is called when there is unexpected action on the I2C interface.
	 *
	 * This function prints out the expected status code
	 * followed by the actual status code. This function uses the hardware UART.
	 * to transmit the data
	 */
	//void Error(uint8_t expected_status);

	/** Adrress:
	 * Tranmits the passed address and RW bit to the I2C Bus lines
	 */
	void Address(uint8_t address, uint8_t rw_bit);

	/** Transmit:
	 * The value in outbyte	is sent to the device
	 * The device should have already been addressed using
	 * Address(...);
	 */
	void Transmit(uint8_t outbyte);


	/** Read:
	 * The value in receiver buffer is returned.
	 * This function should only be called after SLA+R+ACK has been sent */
	/** Pass a 0 (LAST_BYTE) to the acknowledge bit to end the read
	 * operation.
	 * Most I2C devices require the last byte to be read to end
	 * with a NACK.
	 */
	uint8_t Read(uint8_t acknowledge_bit);

private:
	/** Start:
	 * Place a start bit on the I2C Bus
	 *
	 */
	/** I2CNT is cleared when a 1 is written to it*/
	/** I2C Operations take place when I2CNT is cleared (set to 1) */
	void Start(void);


	/** Stop:
	 * Place a start bit on the I2C Bus
	 *
	 */
	/** I2CNT is cleared when a 1 is written to it*/
	/** I2C Operations take place when I2CNT is cleared (set to 1) */
	void Stop(void);
};


}




#endif
