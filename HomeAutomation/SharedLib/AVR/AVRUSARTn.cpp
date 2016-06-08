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
*  These functions utilise the hardware USART0 registers
*
* Filename: harUart.c
* By Adrian Gin (amg94@student.canterbury.ac.nz)
* Created: 16-06-06
* 
* For AVR USART0 Core
* Since the UART has true ouputs, a MAX232 is required to interface
* with a computer's serial port.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "AVRUSARTn.h"

#include "UART.h"

/* uartInit:
 * Initialises the baudrate, parity, stop bit generation and 8bit mode
 * It must be called before any hardUart function is used 
 *
 * Interrupts are not set by default.
 *
 */

namespace AVR
{


USARTn::USARTn(volatile uint8_t& UCSRnA,
		   volatile uint8_t& UCSRnB,
		   volatile uint8_t& UCSRnC,
		   volatile uint8_t& UBRRnH,
		   volatile uint8_t& UBRRnL,
		   volatile uint8_t& UDRn) noexcept : UCSRnA(UCSRnA), UCSRnB(UCSRnB), UCSRnC(UCSRnC), UBRRnH(UBRRnH), UBRRnL(UBRRnL), UDRn(UDRn)
{

}

void USARTn::Init(uint16_t baudrate)
{
	/*Setup the U2X Bit*/
	UCSRnA	=	(UCSRnA & (~(1<<U2Xn))) | (1 << U2Xn);
	
	UCSRnB |= (1<<RXENn) | (1<<TXENn);	/*Enable Rx and Tx modules*/
	UCSRnB &= ~(1<<UCSZn2);				/*Set to 8bit mode*/
	

	/*Select UCSRC to be written to*/	
	/* Set to Asynchronous Mode
	 *			 1 Stop-bit
	 *			 No Parity
	 *			 8-bit char mode
	 */
	UCSRnC = (UCSRnC & ~( UCSRCMASK )) | (NOPARITY<<UPMn0) | (BIT8 << UCSZn0);


	//Enable interrupt.
	UCSRnB |= (1<<RXCIEn);

	/*Set the baud rate to the desired rate*/
	USARTn::SetBaud(baudrate);
}

/* uartSetBaud:
 * Changes the baudrate to the specified value.
 * See the datasheet for more details on what the
 * Baudrate generation registers should be.
 */
void USARTn::SetBaud(uint16_t baudrate)
{
	UBRRnH = 	(baudrate & 0xFF00) >> 8;
	/* The lower 8bits must be written last as the baudrate generator
	 * is updated as soon as UBRRL is written to*/
	UBRRnL	=	(baudrate & 0x00FF);

}


/* Disables the Receiver and Transmitter modules*/
void USARTn::Disable()
{
	UCSRnB &= ~((1<<RXENn) | (1<<TXENn));	/*Disable Rx and Tx modules*/
	
}

/* uartTx:
 * blocking transmits
 * Transmits the passed byte to the Uart.Tx pin.
 *
 */
void USARTn::tx(uint8_t outbyte)
{
	/*Wait until output shift register is empty*/	
	while( ((UCSRnA) & (1<<UDREn)) == 0 );
		
	/*Send byte to output buffer*/
	UDRn	= outbyte;
}



/* uartTxString:
 * Outputs the passed string to the UART.Tx pin
 * The output is true ouput, not inverted, so a MAX232 or some sort of
 * TTL -> +/- 15V converter is required.
 */
/* Usage: uartTxString_P( PSTR("hello!") ); */
/* Send a string which resides in the program memory */
void USARTn::tx(const char* outString_P)
{
   char c;
   while( (c = pgm_read_byte(outString_P++)) )
   {
	   tx(c);
   }
}


/* To echo the receiver buffer, write this code in the main.c file */
/*
ISR(SIG_UART_RECV)
{
	uartTx(UDR);
}
*/

}

