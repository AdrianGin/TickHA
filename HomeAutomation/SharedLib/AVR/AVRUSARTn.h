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


/* Serial Port routines for the AVR mpu's.
*  These functions utilise the hardware UART registers
*
* Filename: harUart.h
* By Adrian Gin (amg94@student.canterbury.ac.nz)
* Created: 16-06-07
* 
* For AVR Core
* Since the UART has true ouputs, a MAX232 is required to interface
* with a computer's serial port.
*/


/** \file UART.h
	 \brief UART Drivers for the AVR Core.
*/

/**	 

	\ingroup avr_peripheral
 	\defgroup hardUart Hardware UART Routines
 	\code #include "UART.h" \endcode

 	 
	 \par Description
	 Functions which provide an interface to the AVR UART
	 component.
 	 
*/
//@{

#ifndef _AVR_USARTN_H
#define	_AVR_USARTN_H


#include <avr/io.h>
#include <avr/interrupt.h>

#include "UART.h"

#define	SET	1
#define	CLEAR	0

#define	DEFAULT_SPD		8
#define	U2X_BIT_STATUS	SET

/** For a processor at 8MHz and U2X = 1
 */
#if  U2X_BIT_STATUS == SET
#define 	BAUD2400		(416)
#define 	BAUD4800		(207)
#define 	BAUD9600		(103)
#define 	BAUD14400	(68)
#define  BAUD19200	(51)
#define 	BAUD28800	(34)
#define	BAUD38400	(25)
#define	BAUD57600	(16)
#define	BAUD76800	(12)
#define	BAUD115200	(8)
#define	BAUD230400	(3) /** High Error */
#define	BAUD250000	(3)
#define	BAUD500000	(1)
#define	BAUD1000000	(0)

/** For a processor at 8MHz and U2X = 0
 */
#else 
 
#define 	BAUD2400		(207)
#define 	BAUD4800		(103)
#define 	BAUD9600		(51)
#define 	BAUD14400	(34)
#define	BAUD19200	(25)
#define 	BAUD28800	(16)
#define	BAUD38400	(12)
#define	BAUD57600	(8)
#define	BAUD76800	(6)
#define	BAUD115200	(3)
#define	BAUD230400	(1) /*High Error */
#define	BAUD250000	(1)
#define	BAUD500000	(0)

#endif 

#define	BAUD_DEFAULT	BAUD9600
#define	PARITY_MASK			(0x30)
#define	NOPARITY			(0x00)
#define	EVEN				(0x02)
#define	ODD					(0x03)

#define	CHARSIZE_MASK		(0x06)
#define BIT8				(0x03)
#define	BIT7				(0x02)
#define	BIT6				(0x01)
#define	BIT5				(0x00)

#define	UCSRCMASK			(0x7F)


/** uartInit:
 * Initialises the baudrate, parity, stop bit generation and 8bit mode
 * It must be called before any hardUart function is used 
 *
 * Interrupts are not set by default.
 *
 */
#define FAST 1
#define SLOW 0

//UCSRnA Bit defs
#define MPCMn MPCM0
#define U2Xn  U2X0
#define UPEn  UPE0
#define DORn  DOR0
#define FEn   FE0
#define UDREn UDRE0
#define TXCn  TXC0
#define RXCn  RXC0

//UCSRnB Bit defs
#define TXB8n TXB80
#define RXB8n RXB80
#define UCSZn2 UCSZ02
#define TXENn TXEN0
#define RXENn RXEN0
#define UDRIEn UDRIE0
#define TXCIEn TXCIE0
#define RXCIEn RXCIE0


//UCSRnc Bit defs
#define UCPOLn UCPOL0
#define UCSZn0 UCSZ00
#define UCPHAn UCPHA0
#define UCSZn1 UCSZ01
#define UDORDn UDORD0
#define USBSn  USBS0
#define UPMn0  UPM00
#define UPMn1  UPM01
#define UMSELn0 UMSEL00
#define UMSELn1 UMSEL01


namespace AVR
{

class USARTn : public Devices::UART
{

public:

	USARTn(volatile uint8_t& UCSRnA,
		   volatile uint8_t& UCSRnB,
		   volatile uint8_t& UCSRnC,
		   volatile uint8_t& UBRRnH,
		   volatile uint8_t& UBRRnL,
		   volatile uint8_t& UDRn) noexcept;


	void Init(uint16_t baudrate);
	/** uartDisable:
	 * Disables the Receiver and Transmitter modules*/
	void Disable();

	/** uartSetBaud:
	 * Changes the baudrate to the specified value.
	 * See the datasheet for more details on what the
	 * Baudrate generation registers should be.
	 */
	void SetBaud(uint16_t baudrate);

	/** uartTxString:
	 * Outputs the passed string to the UART.Tx pin
	 * The output is true ouput, not inverted, so a MAX232 or some sort of
	 * TTL -> +/- 15V converter is required.
	 */
	void tx(char* outString);

	void tx(const char* outString_P);
	void tx_P(const char* outString_P);

	/** uartTx:
	 *
	 * Transmits the passed byte to the Uart.Tx pin.
	 *
	 */
	void tx(uint8_t outbyte);

	/**
	 *  Prints out nbytes of buffer to the UART
	 */
	void tx(uint8_t* buffer, uint8_t nbytes );

	/* ISR(SIG_UART_RECV)
	 *
	 * The interrupt routine for when a receive is complete
	 */
	//ISR(SIG_UART_RECV);

	void NewLine();


private:
	volatile uint8_t& UCSRnA;
	volatile uint8_t& UCSRnB;
	volatile uint8_t& UCSRnC;

	volatile uint8_t& UBRRnH;
	volatile uint8_t& UBRRnL;

	volatile uint8_t& UDRn;


};



}


#endif
