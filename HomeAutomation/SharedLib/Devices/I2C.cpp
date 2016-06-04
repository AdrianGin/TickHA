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

/* Updated for C++
 *
 */
 
 
#include "I2C.h"



namespace Devices
{


/* AVRTWI::TransmitBlock:
 * Outputs n number of bytes onto the TWI Bus.
 */
void I2C::TransmitBlock(uint8_t* data_ptr, uint8_t n)
{
	uint8_t i;

	for( i = 0; i < n; i++)
	{
		I2C::Transmit(data_ptr[i]);
	}
}


/* AVRTWI::ReadBlock:
 * Reads 'size' number of bytes to the passed pointer
 *
 */
void I2C::ReadBlock(uint8_t* destination_ptr, uint8_t size)
{
	uint8_t i;
	for( i = 0; i < (size-1); i++)
	{
		destination_ptr[i] = I2C::Read( Devices::I2C::ACK_BIT);
	}

	destination_ptr[i] = I2C::Read( Devices::I2C::NACK_BIT);
}




}
