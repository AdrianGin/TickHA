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




#ifndef	_DEVICES_I2C_H
#define	_DEVICES_I2C_H

#include <stdint.h>

namespace Devices
{

class I2C
{

public:

	enum eRWBit
	{
		/** A write corresponds to SDA being pulled low */
		WRITE = 0,
		/** A read corresponds to SDA being pulled high */
		READ,
	};


	enum eACKbits
	{
		NACK_BIT = 0,
		ACK_BIT
	};

	enum eBitRate
	{
		CLK_100KHZ = 0,
		CLK_400KHZ
	};

	/** SetBitRate:
	 * Sets up the  bitrate generator
	 */
	virtual void SetBitRate(uint8_t br) = 0;


	/** Init:
	 * Initialise the I2C modules by enabling the
	 * ACKnowledge bit and the I2C Enable bit
	 *
	 * Set the speed of the bitrate generator as well.
	 */

	virtual void Init(uint8_t bitrate) = 0;

	/** Disable:
	 * Disables the I2C Module access to the SDA and SCL pins
	 *
	 */
	virtual void Disable(void) = 0;


	/** Start:
	 * Place a start bit on the I2C Bus
	 *
	 */
	/** I2CNT is cleared when a 1 is written to it*/
	/** I2C Operations take place when I2CNT is cleared (set to 1) */
	virtual void Start(void) = 0;


	/** Stop:
	 * Place a start bit on the I2C Bus
	 *
	 */
	/** I2CNT is cleared when a 1 is written to it*/
	/** I2C Operations take place when I2CNT is cleared (set to 1) */
	virtual void Stop(void) = 0;


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
	virtual void Address(uint8_t address, uint8_t rw_bit) = 0;

	/** Transmit:
	 * The value in outbyte	is sent to the device
	 * The device should have already been addressed using
	 * Address(...);
	 */
	virtual void Transmit(uint8_t outbyte) = 0;

	/** TransmitBlock:
	 * Outputs n number of bytes onto the I2C Bus.
	 */
	void TransmitBlock(uint8_t* data_ptr, uint8_t n);

	/** Read:
	 * The value in receiver buffer is returned.
	 * This function should only be called after SLA+R+ACK has been sent */
	/** Pass a 0 (LAST_BYTE) to the acknowledge bit to end the read
	 * operation.
	 * Most I2C devices require the last byte to be read to end
	 * with a NACK.
	 */

	virtual uint8_t Read(uint8_t acknowledge_bit) = 0;

	/** ReadBlock:
	 * Reads 'size' number of bytes to the passed pointer
	 *
	 */
	void ReadBlock(uint8_t* destination_ptr, uint8_t size);

private:






};


}
 






#endif
