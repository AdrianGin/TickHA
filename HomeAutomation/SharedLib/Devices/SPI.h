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



/* SPI Routines for the ATMEGA Micro
 * Author: Adrian Gin
 * Date: 27/06/07
 *
 */

/** \file spi.h
	 \brief SPI Drivers for the AVR Core.
*/

/**	 

	\ingroup avr_peripheral
 	\defgroup spi SPI Hardware Driver.
 	\code #include "spi.h" \endcode

 	 
	 \par Description
	 SPI Communications protocol for the AVR core.
 	 
*/
//@{
 

#ifndef	_DEVICES_SPI_H
#define	_DEVICES_SPI_H

#include <stdint.h>

namespace Devices
{

class SPI
{

public:
	enum eTxStates{
		TRANSFER_INCOMPLETE,
		TRANSFER_COMPLETE
	};

	//SPI() noexcept;

	virtual void Init(void) = 0;

	/* Note that these are blocking and non DMA */
	virtual uint8_t TxByte(uint8_t data) = 0;
	/** SPI_RxByte, the same as SPI_TxByte(0xFF) */
	virtual uint8_t RxByte(void) = 0;

	virtual void RxBlock(uint8_t* data, uint8_t n) = 0;
	virtual void TxBlock(uint8_t* data, uint8_t n) = 0;

private:


};


}
 






#endif
