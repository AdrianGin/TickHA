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


#ifndef	_DEVICES_UART_H
#define	_DEVICES_UART_H

#include <stdint.h>


namespace Devices
{

class UART
{

public:

	enum eBaudRates
	{
		BAUD2400,
		BAUD4800,
		BAUD9600,
		BAUD19200,
		BAUD31250, //MIDI
		BAUD38400,
		BAUD57600,
		BAUD115200,
	};

	virtual void Init(uint16_t baudrate) = 0;
	virtual void tx(uint8_t c) = 0;
	virtual void tx(const char* string) = 0;

	/**
	 *  Prints out nbytes of buffer to the UART
	 */
	void tx(uint8_t* buffer, uint8_t nbytes );
	void tx(char* string);
	void tx_newline(void);

private:

};







}













#endif




