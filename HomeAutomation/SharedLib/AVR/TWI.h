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

#ifndef	_I2C_ROUTINES
#define	_I2C_ROUTINES

#include <avr/io.h>

#include <stdint.h>
#include <util/twi.h>


#define	TWI_PRESCALE_MASK		(0x03)
#define	TWI_PRESCALE_0			(0x00)
#define	TWI_PRESCALE_1			(0x01)
#define	TWI_PRESCALE_2		(0x02)
#define	TWI_PRESCALE_3		(0x03)

#define	TWI_100KHZ			(64)
#define	TWI_400KHZ			(34)

#define	TWI_STATUS_MASK		0xF8


/** A read corresponds to SDA being pulled high */ 
#define	READ	1

/** A write corresponds to SDA being pulled low */ 
#define	WRITE 0 

#define	ACK_BIT		1
#define	NACK_BIT 	0

#define	I2C_TRANSMIT_MASK 	((1<<TWINT) | (1<<TWSTA) | (1<<TWEA))
#define	I2C_RECEIVE_MASK 		((1<<TWINT) | (1<<TWSTO) | (1<<TWEA))


/** i2cSetBitRate:
 * Sets up the i2c bitrate generator
 *
 * SCL Frequncy = F_CPU / (16 + 2*(bitrate) * 4^(prescale) ) 
 *
 * For SCL[f] = 100kHz,
 * bitrate = 64, prescale = 3
 *
 * For SCL[f] = 400kHz
 * bitrate = 34, prescale = 3.
 */
void i2cSetBitRate(uint8_t bitrate, uint8_t prescale);


/** i2cInit:
 * Initialise the TWI modules by enabling the 
 * ACKnowledge bit and the TWI Enable bit
 *
 * Set the speed of the bitrate generator as well.
 */

void i2cInit(uint8_t bitrate, uint8_t prescale);

/** i2cDisable:
 * Disables the TWI Module access to the SDA and SCL pins
 *
 */
void i2cDisable(void);


/** i2cStart:
 * Place a start bit on the TWI Bus 
 *
 */	
/** TWINT is cleared when a 1 is written to it*/
/** TWI Operations take place when TWINT is cleared (set to 1) */
void i2cStart(void);


/** i2cStop:
 * Place a start bit on the TWI Bus 
 *
 */	
/** TWINT is cleared when a 1 is written to it*/
/** TWI Operations take place when TWINT is cleared (set to 1) */
void i2cStop(void);





/** i2cError:
 * This is called when there is unexpected action on the TWI interface.
 * 
 * This function prints out the expected status code
 * followed by the actual status code. This function uses the hardware UART.
 * to transmit the data
 */
void i2cError(uint8_t expected_status);



/** i2cTransmit:
 * The value in outbyte	is sent to the device 
 * The device should have already been addressed using
 * i2cAddress(...);
 */
void i2cTransmit(uint8_t outbyte);

/** i2cTransmitBlock:
 * Outputs n number of bytes onto the TWI Bus.
 */
void i2cTransmitBlock(uint8_t* data_ptr, uint8_t n);


/** i2cAdrress:
 * Tranmits the passed address and RW bit to the TWI Bus lines
 */
void i2cAddress(uint8_t address, uint8_t rw_bit);


/** i2cRead:
 * The value in receiver buffer is returned.
 * This function should only be called after SLA+R+ACK has been sent */


/** Pass a 0 (LAST_BYTE) to the acknowledge bit to end the read
 * operation. 
 * Most I2C devices require the last byte to be read to end
 * with a NACK.
 */
 
uint8_t i2cRead(uint8_t acknowledge_bit);



/** i2cReadBlock:
 * Reads 'size' number of bytes to the passed pointer
 *
 */
void i2cReadBlock(uint8_t* destination_ptr, uint8_t size);

#endif
