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
#include <util/delay.h>
#include "USARTn.h"
/* uartInit:
 * Initialises the baudrate, parity, stop bit generation and 8bit mode
 * It must be called before any hardUart function is used 
 *
 * Interrupts are not set by default.
 *
 */

#ifdef __AVR_HAVE_PRR_PRUSART0
USARTn_t USART0 = {
	.UCSRnA = &UCSR0A,
	.UCSRnB = &UCSR0B,
	.UCSRnC = &UCSR0C,
	.UBRRnH = &UBRR0H,
	.UBRRnL = &UBRR0L,
	.UDRn   = &UDR0
};
#endif

#ifdef __AVR_HAVE_PRR_PRUSART1
USARTn_t USART1 = {
	.UCSRnA = &UCSR1A,
	.UCSRnB = &UCSR1B,
	.UCSRnC = &UCSR1C,
	.UBRRnH = &UBRR1H,
	.UBRRnL = &UBRR1L,
	.UDRn   = &UDR1
};
#endif

void USARTn_Init(USARTn_t* uart, uint8_t baudrate, uint8_t U2Xvalue)
{
	/*Setup the U2X Bit*/
	*uart->UCSRnA	=	(*uart->UCSRnA & (~(1<<U2Xn))) | (U2Xvalue << U2Xn);
	
	*uart->UCSRnB |= (1<<RXENn) | (1<<TXENn);	/*Enable Rx and Tx modules*/
	*uart->UCSRnB &= ~(1<<UCSZn2);				/*Set to 8bit mode*/
	

	/*Select UCSRC to be written to*/	
	/* Set to Asynchronous Mode
	 *			 1 Stop-bit
	 *			 No Parity
	 *			 8-bit char mode
	 */
	*uart->UCSRnC = (*uart->UCSRnC & ~( UCSRCMASK )) | (NOPARITY<<UPMn0) | (BIT8 << UCSZn0);

	/*Set the baud rate to the desired rate*/
	USARTn_SetBaud(uart, baudrate, 0);
}

/* uartSetBaud:
 * Changes the baudrate to the specified value.
 * See the datasheet for more details on what the
 * Baudrate generation registers should be.
 */
void USARTn_SetBaud(USARTn_t* uart, uint8_t baudrateL, uint8_t baudrateH)
{
	*uart->UBRRnH = 	baudrateH;
	/* The lower 8bits must be written last as the baudrate generator
	 * is updated as soon as UBRRL is written to*/
	*uart->UBRRnL	=	baudrateL;

}


/* Disables the Receiver and Transmitter modules*/
void USARTn_Disable(USARTn_t* uart)
{
	*uart->UCSRnB &= ~((1<<RXENn) | (1<<TXENn));	/*Disable Rx and Tx modules*/
	
}

/* uartTx:
 *
 * Transmits the passed byte to the Uart.Tx pin.
 *
 */
void USARTn_Tx(USARTn_t* uart, uint8_t outbyte)
{
	/*Wait until output shift register is empty*/	
	while( ((*uart->UCSRnA) & (1<<UDREn)) == 0 );
		
	/*Send byte to output buffer*/
	*(uart->UDRn)	= outbyte;
}

/** Writes nbytes of buffer to the UART */
void USARTn_TxDump(USARTn_t* uart, uint8_t* buffer, uint8_t nbytes )
{
	uint16_t i = 0;
	while( i++ < nbytes )
	{
		USARTn_Tx(uart, *buffer++);
	}
}


/* uartTxString:
 * Outputs the passed string to the UART.Tx pin
 * The output is true ouput, not inverted, so a MAX232 or some sort of
 * TTL -> +/- 15V converter is required.
 */
void USARTn_TxString(USARTn_t* uart, char* outString)
{
	while( *outString )
	{
		USARTn_Tx(uart, *outString++);
   }
	
}

/* Usage: uartTxString_P( PSTR("hello!") ); */
/* Send a string which resides in the program memory */
void USARTn_TxString_P(USARTn_t* uart, const char* outString_P)
{

   char c;

   while( (c = pgm_read_byte(outString_P++)) )
   {
	   USARTn_Tx(uart, c);
   }
}

void USARTn_NewLine(USARTn_t* uart)
{
	USARTn_Tx(uart, '\r');
	USARTn_Tx(uart, '\n');
}

/* To echo the receiver buffer, write this code in the main.c file */
/*
ISR(SIG_UART_RECV)
{
	uartTx(UDR);
}
*/

